/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "move_generator.hpp"
#include "evaluator.hpp"
#include "io.hpp"
#include <chrono>
#include <iomanip>
#include <tbb/task.h>

using namespace chess;

class transposition_entry
{
public:
	enum flag_t : std::uint8_t
	{
		UNKNOWN,	//!< UNKNOWN
		UPPER,		//!< UPPER
		LOWER,		//!< LOWER
		EXACT		//!< EXACT
//		EGTB		//!< EGTB
	};

private:
	hash_t			_hash;
	move_t			_move;
	score_t			_score;
	std::uint8_t	_depth;
	flag_t			_flag;

public:
	constexpr transposition_entry() noexcept
	:
		_hash(),
		_move(),
		_score(),
		_depth(),
		_flag()
	{
	}

	constexpr transposition_entry(const hash_t hash, const move_t move, const score_t score, const flag_t flag, const std::uint8_t depth) noexcept
	:
		_hash(hash),
		_move(move),
		_score(score),
		_depth(depth),
		_flag(flag)
	{
	}
/*
	constexpr transposition_entry& operator=(const transposition_entry& entry) noexcept
	{
		// Try to make operator=() an atomic instruction.
		*reinterpret_cast<__m128*>(this) = *reinterpret_cast<const __m128* const>(&entry);
		return *this;
	}

	constexpr transposition_entry& operator=(transposition_entry&& entry) noexcept
	{
		// Try to make operator=() an atomic instruction.
		*reinterpret_cast<__m128*>(this) = *reinterpret_cast<__m128*>(&entry);
		return *this;
	}
*/
	constexpr hash_t hash() const noexcept
	{
		return _hash;
	}

	constexpr move_t move() const noexcept
	{
		return _move;
	}

	constexpr score_t score() const noexcept
	{
		return _score;
	}

	constexpr flag_t flag() const noexcept
	{
		return _flag;
	}

	constexpr std::uint8_t depth() const noexcept
	{
		return _depth;
	}
};

class transposition_table_t
{
private:
	std::array<std::vector<transposition_entry>, 2> _entries;

public:
	inline transposition_table_t(const std::size_t size)
//	:
//		_entries{}
	{
		_entries[0].resize(size / sizeof(transposition_entry) / 2 + 13, transposition_entry());
		_entries[1].resize(size / sizeof(transposition_entry) / 2 + 13, transposition_entry());
	}


	inline void clear() noexcept
	{
		std::fill(_entries[0].begin(), _entries[0].end(), transposition_entry());
		std::fill(_entries[1].begin(), _entries[1].end(), transposition_entry());
	}

	inline void put(const node_t& node, const move_t move, const score_t score, const transposition_entry::flag_t flag, const std::uint8_t depth) noexcept
	{
		transposition_entry& entry = operator[](node);
		if (entry.depth() < depth)
			entry = transposition_entry(node.hash(), move, score, flag, depth);
	}

	inline transposition_entry& operator[](const node_t& node) noexcept
	{
		return _entries[node.color() == white][node.hash() % _entries[node.color() == white].size()];
	}

	inline const transposition_entry& operator[](const node_t& node) const noexcept
	{
		return _entries[node.color() == white][node.hash() % _entries[node.color() == white].size()];
	}
};

transposition_table_t t_table(1UL << 33);
history_table_t h_table;
history_table_t b_table;
long count = 0;

constexpr bool
check(const node_t node, const color_t color) noexcept
{
	return color == white
		? node.occupy<king_tag, white_tag>() & node.attack<black_tag>()
		: node.occupy<king_tag, black_tag>() & node.attack<white_tag>();
}

score_t
search(const node_t& node, const score_t alpha, const score_t beta)
{
	count++;

	score_t best_score = alpha;

	const score_t score = evaluator::evaluate(node);
	if (score > best_score)
	{
		if (score >= beta)
			return beta;
		best_score = score;
	}

	const move_generator<active_tag> moves(node);

	for (const auto move : moves)
	{
		const node_t succ(node, move);
		if (check(succ, node.color()))
			continue;
		const score_t score = -search(succ, -beta, -best_score);
		if (score > best_score)
		{
			if (score >= beta)
				return beta;
			best_score = score;
		}
	}

	return best_score;
}

bool
try_null(const node_t& node) noexcept
{
	if (node.color() == white)
		return popcnt(node.occupy<white_tag>()) > 3 /*&& node.boardNBRWhite()*/ && !(node.attack<black_tag>() & detail::attacker<king_tag, white_tag>::attack(node.occupy<white_tag, king_tag>()));
	else
		return popcnt(node.occupy<black_tag>()) > 3 /*&& node.boardNBRBlack()*/ && !(node.attack<white_tag>() & detail::attacker<king_tag, black_tag>::attack(node.occupy<black_tag, king_tag>()));
}

score_t
search(const node_t& node, const score_t alpha, const score_t beta, const int depth, const int height, const move_t moved)
{
	if (depth == 0)
		return search(node, alpha, beta);

	++count;

	move_t best_move = {};
	score_t search_alpha = alpha;
	score_t search_beta = beta;

	const transposition_entry& entry = t_table[node];
	if (entry.hash() == node.hash())
	{
		if (entry.depth() >= depth)
		{
			const score_t score = entry.score();
			switch (entry.flag())
			{
			case transposition_entry::EXACT:
				return score;
			case transposition_entry::LOWER:
				if (score > search_alpha)
					search_alpha = score; // todo
				break;
			case transposition_entry::UPPER:
				if (score < search_beta)
					search_beta = score; // todo
				break;
			default:
				break;
			}
			if (search_alpha >= search_beta)
				return search_beta;
		}
		best_move = entry.move();
	}

	int legal = 0;
	score_t best_score = search_alpha;

	constexpr std::uint_fast8_t reduction = 3;//(3 + (depth > 6));
	if (depth >= reduction && moved != move_t {} && !check(node, node.color()) && try_null(node))
	{
		const square_t enPassant = const_cast<node_t&>(node).flip(0);
		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());//, line);
		const_cast<node_t&>(node).flip(enPassant);
		if (score >= search_beta)
			return search_beta;
	}

	if (best_move == move_t {} && depth > 2)
	{
		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
		const transposition_entry& entry = t_table[node];
		if (entry.hash() == node.hash())
			best_move = entry.move();
	}

	if (best_move != move_t {})
	{
		const node_t succ(node, best_move);
		if (!check(succ, node.color()))
		{
			legal++;
			b_table.put(node, best_move);
			const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
			if (score > best_score)
			{
				if (score >= search_beta)
				{
					t_table.put(node, best_move, search_beta, transposition_entry::LOWER, depth);
//					h_table.put(node, best_move);
					return search_beta;
				}
				best_score = score;
			}
		}
	}

	const move_generator<all_tag> moves(node, h_table, b_table);
	for (const auto move : moves)
	{
		if (move == best_move)
			continue;
		const node_t succ(node, move);
		if (check(succ, node.color()))
			continue;
		legal++;
		b_table.put(node, move);
		score_t score;
		score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
		if (score > best_score && score < search_beta)
			score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
		if (score > best_score)
		{
			if (score >= search_beta)
			{
				t_table.put(node, move, search_beta, transposition_entry::LOWER, depth);
//				h_table.put(node, move);
				return search_beta;
			}
			best_score = score;
			best_move = move;
		}
	}

	if (!legal)
	{
		const score_t score = check(node, node.color()) ? -30000 : 0;
		t_table.put(node, move_t {}, score, transposition_entry::EXACT, depth);
//		const score_t score = check ? -30000 + height : 0;
//		environment.transposition_table().put(node, move_t(), score, transposition_entry::EXACT, depth);
//			environment.transposition_table2().put(node, move_t(), score, transposition_entry::EXACT, depth);
		return score;
	}

	if (best_score > alpha)
	{
		t_table.put(node, best_move, best_score, transposition_entry::EXACT, depth);
		h_table.put(node, best_move);
	}
	else
		t_table.put(node, best_move, best_score, transposition_entry::UPPER, depth);

	return best_score;
}

const auto report_output = [](const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const auto duration, const std::size_t count, const move_t move)//, const iterative_deepening::PV::const_iterator pv_begin, const iterative_deepening::PV::const_iterator pv_end)
{
	const auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(duration);
	std::cout << std::setw(2) << int(iteration) << '/' << std::setw(2) << int(max_height) << ", ";
	std::cout << std::setiosflags(std::ios_base::fixed | std::ios_base::showpos) << std::setw(7) << std::setprecision(2) << float(score / 100.0f) << std::resetiosflags(std::ios_base::showpos) << " p, ";
	std::cout << std::setiosflags(std::ios_base::fixed) << std::setw(6) << std::setprecision(2) << dt.count() << " s, ";
	std::cout << std::setw(10) << count << " N, ";
	std::cout << std::setw(8) << int(count / dt.count()) << " N/s, ";
//	std::copy(pv_begin, pv_end, std::ostream_iterator<std::string>(std::cout, " "));
	std::cout << move;
	std::cout << std::endl;
};

void search(const node_t& node, const int depth, const std::chrono::high_resolution_clock::time_point t0)
{
	score_t score = 0;
	for (int iteration = 1; iteration <= depth; ++iteration)
	{
//		const score_t alpha = score - 50;
//		const score_t beta = score + 50;
//		score = search(node, alpha, beta, iteration);
//
//		if (score <= alpha)
//			score = search(node, -30000, beta, iteration);
//		else if (score >= beta)
//			score = search(node, alpha, +30000, iteration);

		score = search(node, -30000, +30000, iteration, 0, move_t {});

		const auto t1 = std::chrono::high_resolution_clock::now();
		report_output(iteration, 0, score, t1 - t0, count, t_table[node].move());

		if (-30000 + 100 > score || score > +30000 - 100)
			break;
	}
	std::cout << std::endl;
}

const auto now = std::chrono::high_resolution_clock::now;

void test(const node_t& node, const int depth)
{
//	const auto t0 = std::chrono::high_resolution_clock::now();
//	const score_t score = search(node, -30000, +30000, depth);
//	const auto t1 = std::chrono::high_resolution_clock::now();
//	report_output(depth, 0, score, t1 - t0, count, t_table[node].move());

//	tbb::allocate_root_proxy p;
//	tbb::task t = tbb::task::allocate_root();
	search(node, depth, now());
	t_table.clear();
	h_table.clear();
	b_table.clear();
	count = 0;
}

int main()
{
	try
	{
		const node_t node0("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1");
		const node_t node1("rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1");
		const node_t node2("6k1/2b2p1p/ppP3p1/4p3/PP1B4/5PP1/7P/7K w - - 0 1");
		const node_t node3("4r1k1/p1pb1ppp/Qbp1r3/8/1P6/2Pq1B2/R2P1PPP/2B2RK1 b - - 0 1");

		test(node0, 12);
		test(node1, 10);
		test(node2, 18);
		test(node3, 12);
	}
	catch(const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
