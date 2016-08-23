/*
 * searcher.cpp
 *
 *  Created on: 20.08.2016
 *      Author: mike
 */

#include "searcher.hpp"
#include "move_generator.hpp"
#include "evaluator.hpp"
#include "attacker.hpp"
#include "io.hpp"
#include <tbb/task_group.h>
#include <sstream>

namespace chess {

searcher::searcher(transposition_table_t& t_table, const report_t& report) noexcept
:
	_report(report),
	_t_table(t_table),
	_h_table(),
	_b_table(),
	_k_table(),
	_count(),
	_height(),
	_stop(),
	_start()
{
}

score_t
searcher::operator()(const node_t& node, const std::uint_fast8_t depth)
{
	score_t score = 0;
	_start = clock::now();

	for (std::uint_fast8_t iteration = 1; iteration <= depth; ++iteration)
	{
		score = search(node, -30000, +30000, iteration, 0, move_t {});

		if (_stop)
			break;

		_report(iteration, _height, score, clock::now() - _start, _count, get_pv(node));

		if (-30000 + 100 > score || score > +30000 - 100)
			break;
	}

	return score;
}

void
searcher::stop()
{
	_stop = true;
}

score_t
searcher::search(const node_t& node, const score_t alpha, const score_t beta, std::uint_fast8_t depth, const std::uint_fast8_t height, const move_t moved)
{
	if (_stop)
		return 0;

	if (height > _height)
		_height = height;

	if (count_repetition(node) >= 3 || node.half_moves() >= 50)
		return 0;

	const bool check = test_check(node, node.color());
	depth += check;

	if (depth == 0)
		return search(node, alpha, beta);

	++_count;

	move_t best_move = {};
	score_t search_alpha = alpha;
	score_t search_beta = beta;

	const transposition_entry_t& entry = _t_table[node];
	if (entry.hash() == node.hash())
	{
		if (entry.depth() >= depth)
		{
			const score_t score = entry.score();
			switch (entry.flag())
			{
			case transposition_entry_t::EXACT:
				return score;
			case transposition_entry_t::LOWER:
				if (score > search_alpha)
					search_alpha = score;
				break;
			case transposition_entry_t::UPPER:
				if (score < search_beta)
					search_beta = score;
				break;
			default:
				break;
			}
			if (search_alpha >= search_beta)
				return search_beta;
		}
		best_move = entry.move();
	}

	const std::uint_fast8_t reduction = 3 + (depth > 6);
	const bool skip = entry.hash() == node.hash() && entry.depth() >= depth - reduction && entry.score() < search_beta && entry.flag() == transposition_entry_t::UPPER;
	if (depth >= reduction && moved != move_t {} && !check && try_null(node) && !skip)
	{
		const square_t en_passant = const_cast<node_t&>(node).flip(0);
		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());
		const_cast<node_t&>(node).flip(en_passant);
		if (score >= search_beta)
			return search_beta;
	}

	if (best_move == move_t {} && depth > 2)
	{
		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
		const transposition_entry_t& entry = _t_table[node];
		if (entry.hash() == node.hash())
			best_move = entry.move();
	}

	std::atomic_int_fast32_t legal(0);
	score_t best_score = search_alpha;

	if (best_move != move_t {})
	{
		try
		{
			const node_t succ(node, best_move);
			if (!test_check(succ, node.color()))
			{
				legal++;
				_b_table.put(node, best_move);
				const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
				if (score > best_score)
				{
					if (score >= search_beta)
					{
						_t_table.put(node, best_move, search_beta, transposition_entry_t::LOWER, depth);
						_h_table.put(node, best_move);
						_k_table.put(node, best_move, height);
						return search_beta;
					}
					best_score = score;
				}
			}
		}
		catch (...)
		{
			// Illegal move detected. Could come from type-1 collision in TT. Discard "best" move.
			best_move = move_t {};
		}
	}

	bool prune = false;
	score_t scoreX = 0;
	const score_t scoreM = node.score() * node.color();
	switch (depth)
	{
	case 3:
		if (!check && scoreM + 900 <= best_score)
			depth -= 1;
		break;
	case 2:
		scoreX = scoreM + 500;
		if (scoreX <= best_score)
			prune = true;
		break;
	case 1:
		scoreX = scoreM + 333;
		if (scoreX <= best_score)
			prune = true;
		break;
	}

	const move_generator<all_tag> moves(node, _h_table, _b_table, _k_table[height], _k_table[height + 2]);

	if (!check && depth > 3 && moves.size() > 8)
	{
		tbb::task_group group;
		const auto work = [&](const node_t& succ, const move_t move)
		{
			score_t score;
			if (group.is_canceling())
				return;
			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move);
			if (group.is_canceling())
				return;
			if (score > best_score && score < search_beta)
				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
			if (group.is_canceling())
				return;
			if (score > best_score)
			{
				best_score = score;
				best_move = move;
				if (score >= search_beta)
				{
					group.cancel();
					_t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
					_h_table.put(node, move);
					_k_table.put(node, move, height);
				}
			}
		};

		const auto work2 = [&](const node_t& succ, const move_t move)
		{
			const score_t score = -search(succ, -best_score - 1, -best_score, depth - 2, height + 1, move);
			if (score <= best_score)
				return;
			if (group.is_canceling())
				return;
			work(succ, move);
		};

		for (auto move : moves)
		{
			if (group.is_canceling())
				break;
			if (move == best_move)
				continue;
			const node_t succ(node, move);
			if (test_check(succ, node.color()))
				continue;
			legal++;
			_b_table.put(node, move);

			const score_t mscore = moves[move];
			move.index = test_check(succ, succ.color());
			if (!check && !move.index && prune && mscore + scoreX <= best_score)
				continue;

			if (group.is_canceling())
				break;
			if (legal > 4 && depth > 3 && node[move.to] == no_piece && node[move.promotion] == 0 && !check && !move.index)
				group.run(std::bind(work2, succ, move));
			else if (legal > 2)
				group.run(std::bind(work, succ, move));
			else
				work(succ, move);
		}

		group.wait();

		if (best_score >= search_beta)
			return search_beta;
	}
	else
	{
		for (auto move : moves)
		{
			if (move == best_move)
				continue;
			const node_t succ(node, move);
			if (test_check(succ, node.color()))
				continue;
			legal++;
			_b_table.put(node, move);

			const score_t mscore = moves[move];
			move.index = test_check(succ, succ.color());
			if (!check && !move.index && prune && mscore + scoreX <= best_score)
				continue;

			if (legal > 4 && depth > 3 && node[move.to] == no_piece && node[move.promotion] == 0 && !check && !move.index)
			{
				const score_t score = -search(succ, -best_score - 1, -best_score, depth - 2, height + 1, move);
				if (score <= best_score)
					continue;
			}

			score_t score;
			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move);
			if (score > best_score && score < search_beta)
				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
			if (score > best_score)
			{
				if (score >= search_beta)
				{
					_t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
					_h_table.put(node, move);
					_k_table.put(node, move, height);
					return search_beta;
				}
				best_score = score;
				best_move = move;
			}
		}
	}

	if (!legal)
	{
		const score_t score = check ? -30000 + height : 0;
		_t_table.put(node, move_t {}, score, transposition_entry_t::EXACT, depth);
		return score;
	}

	if (best_score > alpha)
	{
		_t_table.put(node, best_move, best_score, transposition_entry_t::EXACT, depth);
		_h_table.put(node, best_move);
	}
	else
		_t_table.put(node, best_move, best_score, transposition_entry_t::UPPER, depth);

	return best_score;
}

score_t
searcher::search(const node_t& node, const score_t alpha, const score_t beta)
{
	if (_stop)
		return 0;

	_count++;

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
		if (test_check(succ, node.color()))
			continue;
		if (node.score() * node.color() + moves[move] + 100 < best_score)
			break;
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

constexpr bool
searcher::test_check(const node_t& node, const color_t color) noexcept
{
	return color == white
		? node.occupy<king_tag, white_tag>() & node.attack<black_tag>()
		: node.occupy<king_tag, black_tag>() & node.attack<white_tag>();
}

inline std::uint_fast8_t
searcher::count_repetition(const node_t& leaf) noexcept
{
	std::uint_fast8_t count = 1;
	for (const node_t* node = leaf.parent(); node && node->half_moves(); node = node->parent())
		count += (leaf.hash() == node->hash());
	return count;
}

template <typename color_tag>
inline bool
searcher::try_null(const node_t& node) noexcept
{
	typedef typename color_traits<color_tag>::other other_tag;
	return popcnt(node.occupy<color_tag>()) > 3
		&& (node.occupy<knight_tag, color_tag>() | node.occupy<bishop_queen_tag, color_tag>() | node.occupy<rook_queen_tag, color_tag>())
		&& !(node.attack<other_tag>() & detail::attacker<king_tag, color_tag>::attack(node.occupy<king_tag, color_tag>()));
}

inline bool
searcher::try_null(const node_t& node) noexcept
{
	return (node.color() == white)
		? try_null<white_tag>(node)
		: try_null<black_tag>(node);
}

std::string
searcher::get_pv(node_t node) const
{
	std::ostringstream stream;
	for (std::uint_fast8_t i = 0; i < _height; ++i)
	{
		const transposition_entry_t& entry = _t_table[node];
		if (entry.hash() != node.hash())
			break;
		const move_t move = entry.move();
		if (move == move_t {})
			break;
		stream << std::make_pair(std::cref(node), std::cref(move)) << ' ';
		node = node_t(node, move);
	}
	return std::move(stream.str());
}

}
