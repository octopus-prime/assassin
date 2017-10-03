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
#include <parallel/algorithm>

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
//	const bool check = test_check(node, node.color());

	score_t score = 0;
	_start = clock::now();

	for (std::uint_fast8_t iteration = 1; iteration <= depth; ++iteration)
	{
		score = search(node, -30000, +30000, iteration, 0, move_t {});

		if (_stop)
			break;

		_report(iteration, _height, score * node.color(), clock::now() - _start, _count, get_pv(node, iteration));

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

const score_t razor_margin[4] = { 483, 570, 603, 554 };

constexpr score_t
futility_margin(std::uint_fast8_t d) noexcept
{
	return 150 * d;
}

bool
danger(const node_t& node, const move_t move) noexcept
{
	switch (node[move.to])
	{
	case P:
		return rank_of(move.to) >= 5;
	case p:
		return rank_of(move.to) <= 2;
	default:
		return false;
	}
}

score_t
searcher::search(const node_t& node, score_t alpha, score_t beta, std::uint_fast8_t depth, const std::uint_fast8_t height, const move_t moved)
{
	if (_stop)
		return 0;

	if (height > _height)
		_height = height;

	if (node.is_3fold_repetition() || node.is_50_moves())
		return 0;

	const bool check = test_check(node, node.color());
	depth += check;

	move_t best_move = {};
	score_t best_score = -30000;

	const transposition_table_t::entry_t* const entry = _t_table[node];
	if (entry)
	{
		if (entry->depth >= depth)
		{
			const score_t score = entry->score;
			switch (entry->flag)
			{
			case transposition_table_t::EXACT:
				return score;
			case transposition_table_t::LOWER:
				if (score > alpha)
					alpha = score;
				break;
			case transposition_table_t::UPPER:
				if (score < beta)
					beta = score;
				break;
			default:
				break;
			}
			if (alpha >= beta)
				return alpha;
		}
		best_move = entry->move;
	}

	if (depth == 0)
		return search(node, alpha, beta);

	++_count;

//	score_t best_score = alpha;//-30000;

//	const bool check = test_check(node, node.color());
	const score_t eval = node.score() * node.color();

    if (!check && depth < 4 && eval + razor_margin[depth] <= alpha && best_move == move_t {})
    {
    	const score_t ralpha = alpha - razor_margin[depth];
    	const score_t score = search(node, ralpha, ralpha+1);
        if (score <= ralpha)
            return score;
    }

    if (!check && depth < 7 && eval - futility_margin(depth) >= beta && eval < 20000)//VALUE_KNOWN_WIN &&  pos.non_pawn_material(pos.side_to_move()))
    {
    	return eval - futility_margin(depth);
    }

	const std::uint_fast8_t reduction = 3 + (depth > 6);
	const bool skip = entry && entry->depth >= depth - reduction && entry->score < beta && entry->flag == transposition_table_t::UPPER;
	if (depth >= reduction && moved != move_t {} && !check && try_null(node) && !skip)
	{
		const square_t en_passant = const_cast<node_t&>(node).flip(0);
		const score_t score = -search(node, -beta, -beta + 1, depth - reduction, height + 1, move_t());
		const_cast<node_t&>(node).flip(en_passant);
		if (score >= beta)
			return beta;
	}

	if (best_move == move_t {} && depth > 2)
	{
		search(node, alpha, beta, depth - 2, height + 1, best_move);
		const transposition_table_t::entry_t* const entry = _t_table[node];
		if (entry)
			best_move = entry->move;
	}

	std::atomic_int_fast32_t legal(0);

	if (best_move != move_t {})
	{
		try
		{
			const node_t succ(node, best_move);
			if (!test_check(succ, node.color()))
			{
				legal++;
				_b_table.put(node, best_move);

				auto ext = 0;//best_move.index;

				const score_t score = -search(succ, -beta, -alpha, depth - 1 + ext, height + 1, best_move);
				if (score > best_score)
				{
					if (score >= beta)
					{
						_t_table.put(node, best_move, score, transposition_table_t::LOWER, depth + ext);
						_h_table.put(node, best_move);
						_k_table.put(node, best_move, height);
						return score;
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

	constexpr killer_table_t::entry_t k_entry {};
	const move_generator<all_tag> moves(node, _h_table, _b_table, _k_table[height], _k_table[height + 2], height > 2 ? _k_table[height - 2] : k_entry);

	auto foo = [&](move_t move) -> bool
	{
		if (move == best_move)
			return false;
		const node_t succ(node, move);
		if (test_check(succ, node.color()))
			return false;
		legal++;
		_b_table.put(node, move);

		const score_t mscore = moves.score(move);
		const bool killer = moves.killer(move);

		move.metadata.index = test_check(succ, succ.color());

		auto ext = //move.index ||
				moves.size() == 1 || danger(node, move);

		score_t score;
		if (best_score > alpha)
		{

			if (legal > 4 && depth > 3 && mscore <= 0 && !check && !move.metadata.index && !killer)
//			if (legal > 4 && depth > 3 && node[move.to] == no_piece && node[move.promotion] == 0 && !check && !move.index)// && !killer)
			{
				const std::uint_fast8_t reduction = 2 + (depth > 6) + (mscore < 0);
				const score_t score = -search(succ, -best_score - 1, -best_score, depth - reduction, height + 1, move);
				if (score <= best_score)
					return false;
			}

			score = -search(succ, -best_score - 1, -best_score, depth - 1 + ext, height + 1, move);
			if (score > best_score && score < beta)
				score = -search(succ, -beta, -best_score, depth - 1 + ext, height + 1, move);
		}
		else
			score = -search(succ, -beta, -alpha, depth - 1 + ext, height + 1, move);
		if (score > best_score)
		{
			best_score = score;
			best_move = move;
			_t_table.put(node, move, score, transposition_table_t::LOWER, depth + ext);
			_h_table.put(node, move);
			_k_table.put(node, move, height);
			return score >= beta;
		}

		return false;
	};

	auto found = std::__parallel::find_if(moves.begin(), moves.end(), foo);

	if (found) {
		return best_score;
	}

	if (!legal)
	{
		const score_t score = check ? -30000 + height : 0;
		_t_table.put(node, move_t {}, score, transposition_table_t::EXACT, depth);
		return score;
	}

	if (best_score > alpha)
	{
		_t_table.put(node, best_move, best_score, transposition_table_t::EXACT, depth);
		_h_table.put(node, best_move);
	}
	else
		_t_table.put(node, best_move, best_score, transposition_table_t::UPPER, depth);

	return best_score;
}

score_t
searcher::search(const node_t& node, const score_t alpha, const score_t beta)
{
	if (_stop)
		return 0;

	_count++;

	score_t best_score = evaluator::evaluate(node);
	if (best_score >= beta)
		return best_score;

	const move_generator<active_tag> moves(node, _h_table, _b_table);

	for (const auto move : moves)
	{
		const node_t succ(node, move);
		if (test_check(succ, node.color()))
			continue;
		const score_t score = -search(succ, -beta, -best_score);
		if (score > best_score)
		{
			if (score >= beta)
				return score;
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

//inline std::uint_fast8_t
//searcher::count_repetition(const node_t& leaf) noexcept
//{
//	std::uint_fast8_t count = 1;
//	for (const node_t* node = leaf.parent(); node && node->half_moves(); node = node->parent())
//		count += (leaf.hash() == node->hash());
//	return count;
//}

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
searcher::get_pv(node_t node, const std::uint_fast8_t length) const
{
	std::ostringstream stream;
	for (std::uint_fast8_t i = 0; i < length; ++i)
	{
		const transposition_table_t::entry_t* const entry = _t_table[node];
		if (!entry)
			break;
		const move_t move = entry->move;
		if (move == move_t {})
			break;
		stream << std::make_pair(std::cref(node), std::cref(move)) << ' ';
		node = node_t(node, move);
	}
	return std::move(stream.str());
}

}
