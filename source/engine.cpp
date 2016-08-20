/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

//#include "move_generator.hpp"
//#include "transposition_table.hpp"
//#include "history_table.hpp"
//#include "killer_table.hpp"
//#include "evaluator.hpp"
#include "searcher.hpp"
#include "io.hpp"
#include <chrono>
#include <iomanip>
#include <future>
//#include <functional>
//#include <atomic>
//#include <tbb/task_group.h>

using namespace chess;


//transposition_table_t t_table(24UL << 30);
//history_table_t h_table;
//history_table_t b_table;
//killer_table_t k_table;
//std::atomic_uint_fast64_t count(0);
//std::atomic_uint_fast8_t max_height(0);
//std::atomic_uint_fast64_t count1(0);
//std::atomic_uint_fast64_t count2(0);
//
//struct environment_t
//{
//	transposition_table_t t_table;
//	history_table_t h_table;
//	history_table_t b_table;
//	killer_table_t k_table;
//	std::atomic_uint_fast64_t count;
//	std::atomic_uint_fast8_t height;
//
//	environment_t(const std::size_t size)
//	:
//		t_table(size),
//		h_table(),
//		b_table(),
//		k_table(),
//		count(),
//		height()
//	{
//	}
//
//	void clear()
//	{
//		t_table.clear();
//		h_table.clear();
//		b_table.clear();
//		k_table.clear();
//		count = 0;
//		height = 0;
//	}
//};
//
//constexpr bool
//test_check(const node_t& node, const color_t color) noexcept
//{
//	return color == white
//		? node.occupy<king_tag, white_tag>() & node.attack<black_tag>()
//		: node.occupy<king_tag, black_tag>() & node.attack<white_tag>();
//}
//
//inline std::uint_fast8_t
//repetition(const node_t& leaf) noexcept
//{
//	std::uint_fast8_t count = 1;
//	for (const node_t* node = leaf.parent(); node && node->half_moves(); node = node->parent())
//		count += (leaf.hash() == node->hash());
//	return count;
//}
//
//score_t
//search(const node_t& node, const score_t alpha, const score_t beta)
//{
//	count++;
//
//	score_t best_score = alpha;
//
//	const score_t score = evaluator::evaluate(node);
//	if (score > best_score)
//	{
//		if (score >= beta)
//			return beta;
//		best_score = score;
//	}
//
//	const move_generator<active_tag> moves(node);
//
//	for (const auto move : moves)
//	{
//		const node_t succ(node, move);
//		if (test_check(succ, node.color()))
//			continue;
//		const score_t mscore = node.score() * node.color() + moves[move];
////		if (mscore > best_score)
////		{
////			if (mscore >= beta)
////				return beta;
////			best_score = mscore;
////		}
//		if (mscore + 100 < best_score)
//			break;
//		const score_t score = -search(succ, -beta, -best_score);
//		if (score > best_score)
//		{
//			if (score >= beta)
//				return beta;
//			best_score = score;
//		}
//	}
//
//	return best_score;
//}
//
////score_t
////search(const node_t& node, const score_t alpha, const score_t beta)
////{
////	count++;
////
////	score_t best_score = alpha;
////
////	const score_t score = evaluator::evaluate(node);
////	if (score > best_score)
////	{
////		if (score >= beta)
////			return beta;
////		best_score = score;
////	}
////
////	const move_generator<active_tag> moves(node);
////
////	for (const auto move : moves)
////	{
////		const node_t succ(node, move);
////		if (test_check(succ, node.color()))
////			continue;
////		const score_t score = -search(succ, -beta, -best_score);
////		if (score > best_score)
////		{
////			if (score >= beta)
////				return beta;
////			best_score = score;
////		}
////	}
////
////	return best_score;
////}
//
//template <typename color_tag>
//bool
//try_null(const node_t& node) noexcept
//{
//	typedef typename color_traits<color_tag>::other other_tag;
//	return popcnt(node.occupy<color_tag>()) > 3
//		&& (node.occupy<knight_tag, color_tag>() | node.occupy<bishop_queen_tag, color_tag>() | node.occupy<rook_queen_tag, color_tag>())
//		&& !(node.attack<other_tag>() & detail::attacker<king_tag, color_tag>::attack(node.occupy<king_tag, color_tag>()));
//}
//
//bool
//try_null(const node_t& node) noexcept
//{
//	return (node.color() == white)
//		? try_null<white_tag>(node)
//		: try_null<black_tag>(node);
//}
///*
//score_t
//search(const node_t& node, const score_t alpha, const score_t beta, const int depth, const int height, const move_t moved)
//{
//	if (depth == 0)
//		return search(node, alpha, beta);
//
//	++count;
//
//	move_t best_move = {};
//	score_t search_alpha = alpha;
//	score_t search_beta = beta;
//
//	const transposition_entry_t& entry = t_table[node];
//	if (entry.hash() == node.hash())
//	{
//		if (entry.depth() >= depth)
//		{
//			const score_t score = entry.score();
//			switch (entry.flag())
//			{
//			case transposition_entry_t::EXACT:
//				return score;
//			case transposition_entry_t::LOWER:
//				if (score > search_alpha)
//					search_alpha = score;
//				break;
//			case transposition_entry_t::UPPER:
//				if (score < search_beta)
//					search_beta = score;
//				break;
//			default:
//				break;
//			}
//			if (search_alpha >= search_beta)
//				return search_beta;
//		}
//		best_move = entry.move();
//	}
//
//	const std::uint_fast8_t reduction = 3 + (depth > 6);
//	if (depth >= reduction && moved != move_t {} && !test_check(node, node.color()) && try_null(node))
//	{
//		const square_t en_passant = const_cast<node_t&>(node).flip(0);
//		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());
//		const_cast<node_t&>(node).flip(en_passant);
//		if (score >= search_beta)
//			return search_beta;
//	}
//
//	if (best_move == move_t {} && depth > 2)
//	{
//		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
//		const transposition_entry_t& entry = t_table[node];
//		if (entry.hash() == node.hash())
//			best_move = entry.move();
//	}
//
//	int legal = 0;
//	score_t best_score = search_alpha;
//
//	if (best_move != move_t {})
//	{
//		const node_t succ(node, best_move);
//		if (!test_check(succ, node.color()))
//		{
//			legal++;
//			b_table.put(node, best_move);
//			const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
//			if (score > best_score)
//			{
//				if (score >= search_beta)
//				{
//					t_table.put(node, best_move, search_beta, transposition_entry_t::LOWER, depth);
////					h_table.put(node, best_move);
//					k_table.put(node, best_move, height);
//					return search_beta;
//				}
//				best_score = score;
//			}
//		}
//	}
//
//	const move_generator<all_tag> moves(node, h_table, b_table, k_table[height], k_table[height + 2]);
//	for (auto move : moves)
//	{
//		if (move == best_move)
//			continue;
//		const node_t succ(node, move);
//		if (test_check(succ, node.color()))
//			continue;
//		legal++;
//		b_table.put(node, move);
//		move.index = test_check(succ, succ.color());
//
//
//
//
//		score_t eval;
//		if(legal >= 4             &&
//		   depth >= 3                       &&
//		   node[move.to] == no_piece    &&
//		   node[move.promotion] == 0    &&
//		   !test_check(node, node.color()) &&
//		   !move.index)
//		{
//			eval = -search(succ, -search_alpha-1,-search_alpha, depth -2, height + 1, move);
//		}
//		else
//			eval = search_alpha +1;
//
//		if (eval <= search_alpha)
//			continue;
//
//
//
//		score_t score;
//		score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//		if (score > best_score && score < search_beta)
//			score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//		if (score > best_score)
//		{
//			if (score >= search_beta)
//			{
//				t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
////				h_table.put(node, move);
//				k_table.put(node, move, height);
//				return search_beta;
//			}
//			best_score = score;
//			best_move = move;
//		}
//	}
//
//	if (!legal)
//	{
//		const score_t score = test_check(node, node.color()) ? -30000 + height : 0;
//		t_table.put(node, move_t {}, score, transposition_entry_t::EXACT, depth);
//		return score;
//	}
//
//	if (best_score > alpha)
//	{
//		t_table.put(node, best_move, best_score, transposition_entry_t::EXACT, depth);
//		h_table.put(node, best_move);
//	}
//	else
//		t_table.put(node, best_move, best_score, transposition_entry_t::UPPER, depth);
//
//	return best_score;
//}
//*/
///*
//score_t
//search(const node_t& node, const score_t alpha, const score_t beta, const int depth, const int height, const move_t moved)
//{
//	if (depth == 0)
//		return search(node, alpha, beta);
//
//	++count;
//
//	move_t best_move = {};
//	score_t search_alpha = alpha;
//	score_t search_beta = beta;
//
//	const transposition_entry_t& entry = t_table[node];
//	if (entry.hash() == node.hash())
//	{
//		if (entry.depth() >= depth)
//		{
//			const score_t score = entry.score();
//			switch (entry.flag())
//			{
//			case transposition_entry_t::EXACT:
//				return score;
//			case transposition_entry_t::LOWER:
//				if (score > search_alpha)
//					search_alpha = score;
//				break;
//			case transposition_entry_t::UPPER:
//				if (score < search_beta)
//					search_beta = score;
//				break;
//			default:
//				break;
//			}
//			if (search_alpha >= search_beta)
//				return search_beta;
//		}
//		best_move = entry.move();
//	}
//
//	const bool check = test_check(node, node.color());
//	const std::uint_fast8_t reduction = 3 + (depth > 6);
//	if (depth >= reduction && moved != move_t {} && !check && try_null(node))
//	{
//		const square_t en_passant = const_cast<node_t&>(node).flip(0);
//		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());
//		const_cast<node_t&>(node).flip(en_passant);
//		if (score >= search_beta)
//			return search_beta;
//	}
//
//	if (best_move == move_t {} && depth > 2)
//	{
//		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
//		const transposition_entry_t& entry = t_table[node];
//		if (entry.hash() == node.hash())
//			best_move = entry.move();
//	}
//
//	std::atomic_int_fast32_t legal(0);
//	score_t best_score = search_alpha;
//
//	if (best_move != move_t {})
//	{
//		const node_t succ(node, best_move);
//		if (!test_check(succ, node.color()))
//		{
//			legal++;
//			b_table.put(node, best_move);
//			const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
//			if (score > best_score)
//			{
//				if (score >= search_beta)
//				{
//					t_table.put(node, best_move, search_beta, transposition_entry_t::LOWER, depth);
////					h_table.put(node, best_move);
//					k_table.put(node, best_move, height);
//					return search_beta;
//				}
//				best_score = score;
//			}
//		}
//	}
//
//	const move_generator<all_tag> moves(node, h_table, b_table, k_table[height]);//, k_table[height + 2]);
//	if (!check && depth > 4 && moves.size() > 8)
//	{
//		tbb::task_group_context context;
//		const auto work = [&](const move_t move)
//		{
//			if (move == best_move)
//				return;
//			const node_t succ(node, move);
//			if (test_check(succ, node.color()))
//				return;
//			legal++;
//			b_table.put(node, move);
//			score_t score;
//			if (context.is_group_execution_cancelled())
//				return;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//			if (context.is_group_execution_cancelled())
//				return;
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//			if (context.is_group_execution_cancelled())
//				return;
//			if (score > best_score)
//			{
//				best_score = score;
//				best_move = move;
//				if (score >= search_beta)
//				{
//					context.cancel_group_execution();
//					t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
//	//				h_table.put(node, move);
//					k_table.put(node, move, height);
//				}
//			}
//		};
//
//		tbb::parallel_for_each(moves.begin(), moves.end(), work, context);
//		if (best_score >= search_beta)
//			return search_beta;
//	}
//	else
//	{
//		for (const auto move : moves)
//		{
//			if (move == best_move)
//				continue;
//			const node_t succ(node, move);
//			if (test_check(succ, node.color()))
//				continue;
//			legal++;
//			b_table.put(node, move);
//			score_t score;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//			if (score > best_score)
//			{
//				if (score >= search_beta)
//				{
//					t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
//	//				h_table.put(node, move);
//					k_table.put(node, move, height);
//					return search_beta;
//				}
//				best_score = score;
//				best_move = move;
//			}
//		}
//	}
//
//	if (!legal)
//	{
//		const score_t score = check ? -30000 + height : 0;
//		t_table.put(node, move_t {}, score, transposition_entry_t::EXACT, depth);
//		return score;
//	}
//
//	if (best_score > alpha)
//	{
//		t_table.put(node, best_move, best_score, transposition_entry_t::EXACT, depth);
//		h_table.put(node, best_move);
//	}
//	else
//		t_table.put(node, best_move, best_score, transposition_entry_t::UPPER, depth);
//
//	return best_score;
//}
//*/
///*
//score_t
//search(const node_t& node, const score_t alpha, const score_t beta, const int depth, const int height, const move_t moved)
//{
//	if (depth == 0)
//		return search(node, alpha, beta);
//
//	++count;
//
//	move_t best_move = {};
//	score_t search_alpha = alpha;
//	score_t search_beta = beta;
//
//	const transposition_entry_t& entry = t_table[node];
//	if (entry.hash() == node.hash())
//	{
//		if (entry.depth() >= depth)
//		{
//			const score_t score = entry.score();
//			switch (entry.flag())
//			{
//			case transposition_entry_t::EXACT:
//				return score;
//			case transposition_entry_t::LOWER:
//				if (score > search_alpha)
//					search_alpha = score;
//				break;
//			case transposition_entry_t::UPPER:
//				if (score < search_beta)
//					search_beta = score;
//				break;
//			default:
//				break;
//			}
//			if (search_alpha >= search_beta)
//				return search_beta;
//		}
//		best_move = entry.move();
//	}
//
//	const bool check = test_check(node, node.color());
//	const std::uint_fast8_t reduction = 3 + (depth > 6);
//	if (depth >= reduction && moved != move_t {} && !check && try_null(node))
//	{
//		const square_t en_passant = const_cast<node_t&>(node).flip(0);
//		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());
//		const_cast<node_t&>(node).flip(en_passant);
//		if (score >= search_beta)
//			return search_beta;
//	}
//
//	if (best_move == move_t {} && depth > 2)
//	{
//		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
//		const transposition_entry_t& entry = t_table[node];
//		if (entry.hash() == node.hash())
//			best_move = entry.move();
//	}
//
//	std::atomic_int_fast32_t legal(0);
//	score_t best_score = search_alpha;
//
//	if (best_move != move_t {})
//	{
//		try
//		{
//			const node_t succ(node, best_move);
//			if (!test_check(succ, node.color()))
//			{
//				legal++;
//				b_table.put(node, best_move);
//				const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
//				if (score > best_score)
//				{
//					if (score >= search_beta)
//					{
//						t_table.put(node, best_move, search_beta, transposition_entry_t::LOWER, depth);
//	//					h_table.put(node, best_move);
//						k_table.put(node, best_move, height);
//						return search_beta;
//					}
//					best_score = score;
//				}
//			}
//		}
//		catch (...)
//		{
//			// Illegal move detected. Could come from type-1 collision in TT. Discard "best" move.
//			best_move = move_t {};
//		}
//	}
//
//	const move_generator<all_tag> moves(node, h_table, b_table, k_table[height]);//, k_table[height + 2]);
//	auto move = moves.begin();
//	for (;move != moves.end() && std::distance(moves.begin(), move) != 3; ++move)
//	{
//		if (*move == best_move)
//			continue;
//		const node_t succ(node, *move);
//		if (test_check(succ, node.color()))
//			continue;
//		legal++;
//		b_table.put(node, *move);
//		score_t score;
//		score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, *move); // todo
//		if (score > best_score && score < search_beta)
//			score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, *move);
//		if (score > best_score)
//		{
//			if (score >= search_beta)
//			{
//				t_table.put(node, *move, search_beta, transposition_entry_t::LOWER, depth);
////				h_table.put(node, move);
//				k_table.put(node, *move, height);
//				return search_beta;
//			}
//			best_score = score;
//			best_move = *move;
//		}
//	}
//
//	if (!check && depth > 4 && moves.size() > 8)
//	{
//		tbb::task_group group;
//		const auto work = [&](const node_t& succ, const move_t move)
//		{
//			score_t score;
//			if (group.is_canceling())
//				return;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//			if (group.is_canceling())
//				return;
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//			if (group.is_canceling())
//				return;
//			if (score > best_score)
//			{
//				best_score = score;
//				best_move = move;
//				if (score >= search_beta)
//				{
//					group.cancel();
//					t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
//	//				h_table.put(node, move);
//					k_table.put(node, move, height);
//				}
//			}
//		};
//
////		for (const auto move : moves)
//		for (;move != moves.end(); ++move)
//		{
//			if (group.is_canceling())
//				break;
//			if (*move == best_move)
//				continue;
//			const node_t succ(node, *move);
//			if (test_check(succ, node.color()))
//				continue;
//			legal++;
//			b_table.put(node, *move);
//
//			if (group.is_canceling())
//				break;
//			group.run(std::bind(work, succ, *move));
//		}
//
//		group.wait();
//
//		if (best_score >= search_beta)
//			return search_beta;
//	}
//	else
//	{
////		for (const auto move : moves)
//		for (;move != moves.end(); ++move)
//		{
//			if (*move == best_move)
//				continue;
//			const node_t succ(node, *move);
//			if (test_check(succ, node.color()))
//				continue;
//			legal++;
//			b_table.put(node, *move);
//			score_t score;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, *move); // todo
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, *move);
//			if (score > best_score)
//			{
//				if (score >= search_beta)
//				{
//					t_table.put(node, *move, search_beta, transposition_entry_t::LOWER, depth);
//	//				h_table.put(node, move);
//					k_table.put(node, *move, height);
//					return search_beta;
//				}
//				best_score = score;
//				best_move = *move;
//			}
//		}
//	}
//
//	if (!legal)
//	{
//		const score_t score = check ? -30000 + height : 0;
//		t_table.put(node, move_t {}, score, transposition_entry_t::EXACT, depth);
//		return score;
//	}
//
//	if (best_score > alpha)
//	{
//		t_table.put(node, best_move, best_score, transposition_entry_t::EXACT, depth);
//		h_table.put(node, best_move);
//	}
//	else
//		t_table.put(node, best_move, best_score, transposition_entry_t::UPPER, depth);
//
//	return best_score;
//}
//*/
//
//
//
//
//template<typename T>
//void update_maximum(std::atomic<T>& maximum_value, T const value) noexcept
//{
//    T prev_value = maximum_value;
//    while (prev_value < value && !maximum_value.compare_exchange_weak(prev_value, value));
//}
//
//score_t
//search(const node_t& node, const score_t alpha, const score_t beta, std::uint_fast8_t depth, const std::uint_fast8_t height, const move_t moved)
//{
//	if (height > max_height)
//		max_height = height;
//
//	if (repetition(node) >= 3 || node.half_moves() >= 50) // todo don't parallel
//		return 0;
//
//	const bool check = test_check(node, node.color());
//	depth += check;
////	depth += bool(moved.promotion);
//
//	if (depth == 0)
//		return search(node, alpha, beta);
//
//	++count;
//
//	move_t best_move = {};
//	score_t search_alpha = alpha;
//	score_t search_beta = beta;
////	transposition_entry_t::flag_t flag = transposition_entry_t::LOWER;
//
//	const transposition_entry_t& entry = t_table[node];
//	if (entry.hash() == node.hash())
//	{
//		if (entry.depth() >= depth)
//		{
//			const score_t score = entry.score();
//			switch (entry.flag())
//			{
//			case transposition_entry_t::EXACT:
//				return score;
//			case transposition_entry_t::LOWER:
//				if (score > search_alpha)
//					search_alpha = score;
//				break;
//			case transposition_entry_t::UPPER:
//				if (score < search_beta)
//					search_beta = score;
//				break;
//			default:
//				break;
//			}
//			if (search_alpha >= search_beta)
//				return search_beta;
//		}
//		best_move = entry.move();
////		flag = entry.flag();
//	}
//
//	//if (tt_depth >= depth - R - 1 && tt_score < beta && tt_type & UPPER) skip_null(); // If a plain reduced search without nullmove won't produce a cutoff...
//
//	const std::uint_fast8_t reduction = 3 + (depth > 6);
//	const bool skip = entry.hash() == node.hash() && entry.depth() >= depth - reduction && entry.score() < search_beta && entry.flag() == transposition_entry_t::UPPER;
//
//	if (depth >= reduction && moved != move_t {} && !check && try_null(node) && !skip)
//	{
//		const square_t en_passant = const_cast<node_t&>(node).flip(0);
//		const score_t score = -search(node, -search_beta, -search_beta + 1, depth - reduction, height + 1, move_t());
//		const_cast<node_t&>(node).flip(en_passant);
//		if (score >= search_beta)
//			return search_beta;
//	}
//
//	if (best_move == move_t {} && depth > 2)
//	{
//		search(node, search_alpha, search_beta, depth - 2, height + 1, best_move);
//		const transposition_entry_t& entry = t_table[node];
//		if (entry.hash() == node.hash())
//			best_move = entry.move();
//	}
//
//	std::atomic_int_fast32_t legal(0);
//	score_t best_score = search_alpha;
//
//	if (best_move != move_t {})
//	{
//		try
//		{
//			const node_t succ(node, best_move);
//			if (!test_check(succ, node.color()))
//			{
//				legal++;
//				b_table.put(node, best_move);
//				const score_t score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, best_move);
//				if (score > best_score)
//				{
//					if (score >= search_beta)
//					{
//						t_table.put(node, best_move, search_beta, transposition_entry_t::LOWER, depth);
//						h_table.put(node, best_move);
//						k_table.put(node, best_move, height);
//						return search_beta;
//					}
//					best_score = score;
//				}
//			}
//		}
//		catch (...)
//		{
//			// Illegal move detected. Could come from type-1 collision in TT. Discard "best" move.
//			best_move = move_t {};
//		}
//	}
//
//	bool prune = false;
//	score_t scoreX = 0;
//	const score_t scoreM = node.score() * node.color();
//	switch (depth)
//	{
//	case 3:
//		if (!check && scoreM + 900 <= best_score)
//			depth -= 1;
//		break;
//	case 2:
//		scoreX = scoreM + 500;
//		if (scoreX <= best_score)
//			prune = true;
//		break;
//	case 1:
//		scoreX = scoreM + 333;
//		if (scoreX <= best_score)
//			prune = true;
//		break;
//	}
//
////	constexpr std::array<score_t, 5> promotion
////	{
////		0,
////		score_of[N] - score_of[P],
////		score_of[B] - score_of[P],
////		score_of[R] - score_of[P],
////		score_of[Q] - score_of[P]
////	};
//
//	const move_generator<all_tag> moves(node, h_table, b_table, k_table[height], k_table[height + 2]);
//
//	if (!check && depth > 3 && moves.size() > 8)// && flag != transposition_entry_t::LOWER)
//	{
//		tbb::task_group group;
//		const auto work = [&](const node_t& succ, const move_t move)
//		{
//			score_t score;
//			if (group.is_canceling())
//				return;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//			if (group.is_canceling())
//				return;
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//			if (group.is_canceling())
//				return;
//			if (score > best_score)
//			{
//				best_score = score;
//				best_move = move;
//				if (score >= search_beta)
//				{
//					group.cancel();
//					t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
//					h_table.put(node, move);
//					k_table.put(node, move, height);
//				}
//			}
//		};
//
//		const auto work2 = [&](const node_t& succ, const move_t move)
//		{
//			const score_t score = -search(succ, -best_score - 1, -best_score, depth - 2, height + 1, move);
//			if (score <= best_score)
//				return;
//			if (group.is_canceling())
//				return;
//			work(succ, move);
//		};
//
////		++count1;
////		bool erfbherje = false;
//		for (auto move : moves)
//		{
//			if (group.is_canceling())
//				break;
//			if (move == best_move)
//				continue;
//			const node_t succ(node, move);
//			if (test_check(succ, node.color()))
//				continue;
//			legal++;
//			b_table.put(node, move);
//
//			const score_t mscore = moves[move];
//			move.index = test_check(succ, succ.color());
//			if (!check && !move.index && prune && mscore + scoreX <= best_score)
//				continue;
//
//			if (group.is_canceling())
//				break;
//			if (legal > 4 && depth > 3 && node[move.to] == no_piece && node[move.promotion] == 0 && !check && !move.index)
//				group.run(std::bind(work2, succ, move));
//			else if (legal > 2)
//				group.run(std::bind(work, succ, move));
//			else
//				work(succ, move);
//
////			if (legal > 2)
////			{
//////				erfbherje = true;
////			}
////			else
////			{
////				work(succ, move);
////			}
//		}
//
//		group.wait();
//
////		count2 += erfbherje;
//
//		if (best_score >= search_beta)
//			return search_beta;
//	}
//	else
//	{
//		for (auto move : moves)
//		{
//			if (move == best_move)
//				continue;
//			const node_t succ(node, move);
//			if (test_check(succ, node.color()))
//				continue;
//			legal++;
//			b_table.put(node, move);
//
//			const score_t mscore = moves[move];
//			move.index = test_check(succ, succ.color());
//			if (!check && !move.index && prune && mscore + scoreX <= best_score)
//				continue;
//
//			if (legal > 4 && depth > 3 && node[move.to] == no_piece && node[move.promotion] == 0 && !check && !move.index)
//			{
//				const score_t score = -search(succ, -best_score - 1, -best_score, depth - 2, height + 1, move);
//				if (score <= best_score)
//					continue;
//			}
//
//			score_t score;
//			score = -search(succ, -best_score - 1, -best_score, depth - 1, height + 1, move); // todo
//			if (score > best_score && score < search_beta)
//				score = -search(succ, -search_beta, -best_score, depth - 1, height + 1, move);
//			if (score > best_score)
//			{
//				if (score >= search_beta)
//				{
//					t_table.put(node, move, search_beta, transposition_entry_t::LOWER, depth);
//					h_table.put(node, move);
//					k_table.put(node, move, height);
//					return search_beta;
//				}
//				best_score = score;
//				best_move = move;
//			}
//		}
//	}
//
//	if (!legal)
//	{
//		const score_t score = check ? -30000 + height : 0;
//		t_table.put(node, move_t {}, score, transposition_entry_t::EXACT, depth);
//		return score;
//	}
//
//	if (best_score > alpha)
//	{
//		t_table.put(node, best_move, best_score, transposition_entry_t::EXACT, depth);
//		h_table.put(node, best_move);
//	}
//	else
//		t_table.put(node, best_move, best_score, transposition_entry_t::UPPER, depth);
//
//	return best_score;
//}
//
////const auto report_output = [](const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const auto duration, const std::size_t count, const move_t move)//, const iterative_deepening::PV::const_iterator pv_begin, const iterative_deepening::PV::const_iterator pv_end)
const auto report_output = [](const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const auto duration, const std::size_t count, const std::string& pv)
{
	const auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(duration);
	std::cout << std::setw(2) << int(iteration) << '/' << std::setw(2) << int(max_height) << ", ";
	std::cout << std::setiosflags(std::ios_base::fixed | std::ios_base::showpos) << std::setw(7) << std::setprecision(2) << float(score / 100.0f) << std::resetiosflags(std::ios_base::showpos) << " p, ";
	std::cout << std::setiosflags(std::ios_base::fixed) << std::setw(6) << std::setprecision(2) << dt.count() << " s, ";
	std::cout << std::setw(10) << count << " N, ";
	std::cout << std::setw(8) << int(count / dt.count()) << " N/s, ";
//	std::copy(pv_begin, pv_end, std::ostream_iterator<std::string>(std::cout, " "));

	std::cout << pv << std::endl;
};
//
//const auto now = std::chrono::high_resolution_clock::now;
//
//void search(const node_t& node, const int depth, const std::chrono::high_resolution_clock::time_point t0)
//{
//	score_t score = 0;
//	for (int iteration = 1; iteration <= depth; ++iteration)
//	{
////		const score_t alpha = score - 25;
////		const score_t beta = score + 25;
////		score = search(node, alpha, beta, iteration, 0, move_t {});
////
////		if (score <= alpha)
////			score = search(node, -30000, beta, iteration, 0, move_t {});
////		else if (score >= beta)
////			score = search(node, alpha, +30000, iteration, 0, move_t {});
//
//		score = search(node, -30000, +30000, iteration, 0, move_t {});
//
//		report_output(iteration, max_height, score, now() - t0, count, node);
//
//		if (-30000 + 100 > score || score > +30000 - 100)
//			break;
//	}
//	std::cout << std::endl;
//}
//
//void test(const node_t& node, const int depth)
//{
//	search(node, depth, now());
//	t_table.clear();
//	h_table.clear();
//	b_table.clear();
//	count = 0;
//	count1 = 0;
//	count2 = 0;
//	max_height = 0;
//}

void test(transposition_table_t & t_table, const node_t& node, const int depth)
{
	searcher search(t_table, report_output);
	const auto x = [&search](const node_t& node, const int depth) {search(node, depth);};
	const std::future<void> work = std::async(std::launch::async, x, node, 255);//depth);
	const std::future_status status = work.wait_for(std::chrono::seconds(90));
	if (status == std::future_status::timeout)
		search.stop();
	work.wait();
	t_table.clear();
//	std::cout << std::endl;
}

void foo()
{
	static const std::initializer_list<std::pair<std::string, std::string>> bt2630
	{
		{"rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1",					"Nf5xg7"},
		{"6k1/2b2p1p/ppP3p1/4p3/PP1B4/5PP1/7P/7K w - - 0 1",						"Bd4xb6"},
		{"5r1k/p1q2pp1/1pb4p/n3R1NQ/7P/3B1P2/2P3P1/7K w - - 0 1", 					"Re5-e6"},
		{"5r1k/1P4pp/3P1p2/4p3/1P5P/3q2P1/Q2b2K1/B3R3 w - - 0 1", 					"Qa2-f7"},
		{"3B4/8/2B5/1K6/8/8/3p4/3k4 w - - 0 1", 									"Kb5-a6"},
		{"1k1r4/1pp4p/2n5/P6R/2R1p1r1/2P2p2/1PP2B1P/4K3 b - - 0 1",					"pe4-e3"},
		{"6k1/p3q2p/1nr3pB/8/3Q1P2/6P1/PP5P/3R2K1 b - - 0 1",						"rc6-d6"},
		{"2krr3/1p4pp/p1bRpp1n/2p5/P1B1PP2/8/1PP3PP/R1K3B1 w - - 0 1",				"Rd6xc6+"},
		{"r5k1/pp2p1bp/6p1/n1p1P3/2qP1NP1/2PQB3/P5PP/R4K2 b - - 0 1",				"pg6-g5"},
		{"2r3k1/1qr1b1p1/p2pPn2/nppPp3/8/1PP1B2P/P1BQ1P2/5KRR w - - 0 1",			"Rg1xg7+"},
		{"1br3k1/p4p2/2p1r3/3p1b2/3Bn1p1/1P2P1Pq/P3Q1BP/2R1NRK1 b - - 0 1",			"qh3xh2+"},
		{"8/pp3k2/2p1qp2/2P5/5P2/1R2p1rp/PP2R3/4K2Q b - - 0 1",						"qe6-e4"},
		{"2bq3k/2p4p/p2p4/7P/1nBPPQP1/r1p5/8/1K1R2R1 b - - 0 1",					"bc8-e6"},
		{"3r1rk1/1p3pnp/p3pBp1/1qPpP3/1P1P2R1/P2Q3R/6PP/6K1 w - - 0 1",				"Rh3xh7"},
		{"2b1q3/p7/1p1p2kb/nPpN3p/P1P1P2P/6P1/5R1K/5Q2 w - - 0 1",					"Pe4-e5"},
		{"2krr3/pppb1ppp/3b4/3q4/3P3n/2P2N1P/PP2B1P1/R1BQ1RK1 b - - 0 1",			"nh4xg2"},
		{"4r1k1/p1qr1p2/2pb1Bp1/1p5p/3P1n1R/3B1P2/PP3PK1/2Q4R w - - 0 1",			"Qc1xf4"},
		{"8/4p3/8/3P3p/P2pK3/6P1/7b/3k4 w - - 0 1",									"Pd5-d6"},
		{"3r2k1/pp4B1/6pp/PP1Np2n/2Pp1p2/3P2Pq/3QPPbP/R4RK1 b - - 0 1",				"pf4-f3"},
		{"r4rk1/5p2/1n4pQ/2p5/p5P1/P4N2/1qb1BP1P/R3R1K1 w - - 0 1",					"Ra1-a2"},
		{"k7/8/PP1b2P1/K2Pn2P/4R3/8/6np/8 w - - 0 1",								"Re4-e1"},
		{"rnb1k2r/pp2qppp/3p1n2/2pp2B1/1bP5/2N1P3/PP2NPPP/R2QKB1R w KQkq - 0 1",	"Pa2-a3"},
		{"8/7p/8/p4p2/5K2/Bpk3P1/4P2P/8 w - - 0 1",									"Pg3-g4"},
		{"R7/3p3p/8/3P2P1/3k4/1p5p/1P1NKP1P/7q w - - 0 1",							"Pg5-g6"},
		{"8/8/3k1p2/p2BnP2/4PN2/1P2K1p1/8/5b2 b - - 0 1",							"ne5-d3"},
		{"2r3k1/pbr1q2p/1p2pnp1/3p4/3P1P2/1P1BR3/PB1Q2PP/5RK1 w - - 0 1",			"Pf4-f5"},
		{"3r2k1/p2r2p1/1p1B2Pp/4PQ1P/2b1p3/P3P3/7K/8 w - - 0 1",					"Pe5-e6"},
		{"rnb1k1nr/p2p1ppp/3B4/1p1N1N1P/4P1P1/3P1Q2/PqP5/R4Kb1 w kq - 0 1",			"Pe4-e5"},
		{"r1b1kb1r/pp1n1ppp/2q5/2p3B1/Q1B5/2p2N2/PP3PPP/R3K2R w KQkq - 0 1",		"Ke1-c1"},
		{"2k5/2p3Rp/p1pb4/1p2p3/4P3/PN1P1P2/1P2KP1r/8 w - - 0 1",					"Pf3-f4"}
	};

	transposition_table_t t_table(24UL << 30);
	for (const std::pair<std::string, std::string>& x : bt2630)
	{
		std::cout << "Solution = " << x.second << std::endl;
		test(t_table, node_t(x.first), 14);
		std::cout << std::endl;
	}
}

int main()
{
	try
	{
//		environment_t environment(24UL << 30);
		foo();
/*
		const node_t node0("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 1 1");
		const node_t node1("rq2r1k1/5pp1/p7/4bNP1/1p2P2P/5Q2/PP4K1/5R1R w - - 0 1"); // "Nf5xg7"
		const node_t node2("6k1/2b2p1p/ppP3p1/4p3/PP1B4/5PP1/7P/7K w - - 0 1"); // Bd4xb6
		const node_t node3("4r1k1/p1pb1ppp/Qbp1r3/8/1P6/2Pq1B2/R2P1PPP/2B2RK1 b - - 0 1");	// "qd3xf3"
		const node_t node4("7r/2qpkp2/p3p3/6P1/1p2b2r/7P/PPP2QP1/R2N1RK1 b - - 0 1"); // "pf7-f5"
		const node_t node5("r3kb1r/1p1b1p2/p1nppp2/7p/4PP2/qNN5/P1PQB1PP/R4R1K w qk - 0 1"); // "Nc3-b1"
		const node_t node6("2b1kb2/8/8/8/8/8/8/4K3 b - - 0 1"); // "..."
		const node_t node7("4r3/p1p1pPp1/P1P1P1P1/5K2/3p2P1/7p/3P1ppr/3R1nkq w - - 0 1"); // fxe8=N
		const node_t node8("7K/2p1p1p1/2PpP1Pk/6pP/Pp4P1/8/1P1P4/8 w - - 0 1"); // a5
		const node_t node9("n1QBq1k1/5p1p/5KP1/p7/8/8/8/8 w - - 0 1"); // Bc7
		const node_t nodea("r1bq1rk1/pp4bp/2np4/2p1p1p1/P1N1P3/1P1P1NP1/1BP1QPKP/1R3R2 b - - 0 1");//	"bc8-h3+"
		const node_t nodeb("8/2kPR3/5q2/5N2/8/1p1P4/1p6/1K6 w - - 0 1");//	"Nf5-d4"
		const node_t nodec("k7/7R/8/8/8/8/8/1R5K w - - 0 1");//	patt
		test(environment, node0, 14);
		test(environment, node1, 12);
		test(environment, node2, 20);
		test(environment, node3, 14);
		test(environment, node4, 14);
//		test(node5, 14);
//		test(node6, 50);
//		test(node7, 30);
//		test(node8, 30);
//		test(node9, 30);
//		test(nodea, 30);
		test(environment, nodeb, 30);
//		test(environment, nodec, 30);
 */
	}
	catch(const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
	}
	return 0;
}
