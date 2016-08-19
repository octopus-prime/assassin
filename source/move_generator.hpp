/*
 * move_generator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include "attacker.hpp"
#include "history_table.hpp"
#include "killer_table.hpp"
#include "exchanger.hpp"
#include <algorithm>
#include <queue>

namespace chess {

typedef std::array<move_t, 256> moves_t;

namespace detail {

template <typename moves_tag, typename color_tag>
struct masker;

template <typename color_tag>
struct masker<all_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return ~node.occupy<color_tag>();
	}
};

template <typename color_tag>
struct masker<passive_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return ~node.occupy();
	}
};

template <typename color_tag>
struct masker<active_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		return node.occupy<other_tag>();
	}
};

template <typename piece_tag, typename color_tag>
struct generator
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<piece_tag, color_tag>()))
			for (const auto to : bsf(attacker<piece_tag, color_tag>::attack(board_of(from), node.occupy()) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<knight_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<knight_tag, color_tag>()))
			for (const auto to : bsf(attacker<knight_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<king_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		const square_t from = node.king<color_tag>();
		for (const auto to : bsf(attacker<king_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node) & ~node.attack<other_tag>()))
			*moves++ = move_t {from, to};
		return castle<moves_tag>(node, moves);
	}

private:
	template <typename moves_tag>
	static moves_t::iterator
	castle(const node_t& node, moves_t::iterator moves) noexcept
	{
		return moves;
	}
};

template <>
template <>
inline moves_t::iterator
generator<king_tag, white_tag>::castle<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	if ((node.castle() & white_castle_king) && !(node.occupy() & (F1 | G1)) && !(node.attack<black_tag>() & (E1 | F1 | G1)))
		*moves++ = move_t {e1, g1};
	if ((node.castle() & white_castle_queen) && !(node.occupy() & (B1 | C1 | D1)) && !(node.attack<black_tag>() & (C1 | D1 | E1)))
		*moves++ = move_t {e1, c1};
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<king_tag, black_tag>::castle<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	if ((node.castle() & black_castle_king) && !(node.occupy() & (F8 | G8)) && !(node.attack<white_tag>() & (E8 | F8 | G8)))
		*moves++ = move_t {e8, g8};
	if ((node.castle() & black_castle_queen) && !(node.occupy() & (B8 | C8 | D8)) && !(node.attack<white_tag>() & (C8 | D8 | E8)))
		*moves++ = move_t {e8, c8};
	return moves;
}

template <typename color_tag>
struct generator<pawn_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept;

private:
	static moves_t::iterator
	generate_moves(const board_t board, const square_t delta, moves_t::iterator moves) noexcept;

	static moves_t::iterator
	generate_promotions(const square_t from, const square_t to, moves_t::iterator moves) noexcept
	{
		for (std::uint8_t promotion = 4; promotion > 0; --promotion)
			*moves++ = move_t {from, to, promotion};
		return moves;
	}
};

template <>
inline moves_t::iterator
generator<pawn_tag, white_tag>::generate_moves(const board_t board, const square_t delta, moves_t::iterator moves) noexcept
{
	for (const auto to : bsf(board))
	{
		const square_t from(to - delta);
		if (to < a8)
			*moves++ = move_t {from, to};
		else
			moves = generate_promotions(from, to, moves);
	}
	return moves;
}

template <>
inline moves_t::iterator
generator<pawn_tag, black_tag>::generate_moves(const board_t board, const square_t delta, moves_t::iterator moves) noexcept
{
	for (const auto to : bsf(board))
	{
		const square_t from(to + delta);
		if (to > h1)
			*moves++ = move_t {from, to};
		else
			moves = generate_promotions(from, to, moves);
	}
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, white_tag>::generate<active_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	constexpr __v4du mask {~Fa, ~Fh, R7};
	constexpr __v4du shift {7, 9, 8};
	const board_t empty = ~node.occupy();
	const board_t attack = node.occupy<black_tag>() | (board_of(node.en_passant()) & R6);
	const __v4du mask2 {attack, attack, empty};
	const __v4du board = ((node.occupy<pawn_tag, white_tag>() & mask) << shift) & mask2;
	moves = generate_moves(board[0], 7, moves);
	moves = generate_moves(board[1], 9, moves);
	moves = generate_moves(board[2], 8, moves);
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, white_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	constexpr __v4du mask {~Fa, ~Fh, ~0UL, R2};
	constexpr __v4du shift {7, 9, 8, 8};
	const board_t empty = ~node.occupy();
	const board_t attack = node.occupy<black_tag>() | (board_of(node.en_passant()) & R6);
	const __v4du mask2 {attack, attack, empty, empty};
	const __v4du board = ((node.occupy<pawn_tag, white_tag>() & mask) << shift) & mask2;
	moves = generate_moves(board[0], 7, moves);
	moves = generate_moves(board[1], 9, moves);
	moves = generate_moves(board[2], 8, moves);
	moves = generate_moves((board[3] << 8) & empty, 16, moves);
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, black_tag>::generate<active_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	constexpr __v4du mask {~Fh, ~Fa, R2};
	constexpr __v4du shift {7, 9, 8};
	const board_t empty = ~node.occupy();
	const board_t attack = node.occupy<white_tag>() | (board_of(node.en_passant()) & R3);
	const __v4du mask2 {attack, attack, empty};
	const __v4du board = ((node.occupy<pawn_tag, black_tag>() & mask) >> shift) & mask2;
	moves = generate_moves(board[0], 7, moves);
	moves = generate_moves(board[1], 9, moves);
	moves = generate_moves(board[2], 8, moves);
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, black_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	constexpr __v4du mask {~Fh, ~Fa, ~0UL, R7};
	constexpr __v4du shift {7, 9, 8, 8};
	const board_t empty = ~node.occupy();
	const board_t attack = node.occupy<white_tag>() | (board_of(node.en_passant()) & R3);
	const __v4du mask2 {attack, attack, empty, empty};
	const __v4du board = ((node.occupy<pawn_tag, black_tag>() & mask) >> shift) & mask2;
	moves = generate_moves(board[0], 7, moves);
	moves = generate_moves(board[1], 9, moves);
	moves = generate_moves(board[2], 8, moves);
	moves = generate_moves((board[3] >> 8) & empty, 16, moves);
	return moves;
}

}

template <typename moves_tag>
class move_generator
{
public:
	move_generator(const node_t& node)
	:
		_end(generate(node, _moves.begin()))
	{
		for (std::uint8_t index = 0; index < size(); ++index)
		{
			move_t& move = _moves[index];
			move.index = index;
			_evals[index].score = exchanger::evaluate(node, move);
		}

		std::stable_sort
		(
			_moves.begin(), _end,
			[this](const move_t& move1, const move_t& move2)
			{
				return _evals[move1.index].score > _evals[move2.index].score;
			}
		);
	}
/*
	move_generator(const node_t& node, const history_table_t& h_table, const history_table_t& b_table)
	:
		_end(generate(node, _moves.begin()))
	{
		struct eval_t
		{
			score_t score;
			float good;
		};
		std::array<eval_t, 256> evals;

		for (std::uint8_t index = 0; index < size(); ++index)
		{
			move_t& move = _moves[index];
			eval_t& eval = evals[index];
			move.index = index;
			eval.score = detail::evaluate(node, move);
			eval.good = float(h_table(node, move)) / float(b_table(node, move));
		}

		std::stable_sort
		(
			_moves.begin(), _end,
			[&evals](const move_t& move1, const move_t& move2)
			{
				const eval_t& eval1 = evals[move1.index];
				const eval_t& eval2 = evals[move2.index];
				if (eval1.score != eval2.score)
					return eval1.score > eval2.score;
				return eval1.good > eval2.good;
			}
		);
	}
*/
	move_generator(const node_t& node, const history_table_t& h_table, const history_table_t& b_table, const killer_table_t::entry_t& k_entry1, const killer_table_t::entry_t& k_entry2)
	:
		_end(generate(node, _moves.begin()))
	{
		for (std::uint8_t index = 0; index < size(); ++index)
		{
			move_t& move = _moves[index];
			eval_t& eval = _evals[index];
			move.index = index;
			eval.score = exchanger::evaluate(node, move);
			eval.good = float(h_table(node, move)) / float(b_table(node, move));
			eval.killer = (k_entry1[0] == move || k_entry1[1] == move) + (k_entry2[0] == move || k_entry2[1] == move);
		}

		std::stable_sort
		(
			_moves.begin(), _end,
			[this](const move_t& move1, const move_t& move2)
			{
				const eval_t& eval1 = _evals[move1.index];
				const eval_t& eval2 = _evals[move2.index];
				if (eval1.score != eval2.score)
					return eval1.score > eval2.score;
				if (eval1.killer != eval2.killer)
					return eval1.killer > eval2.killer;
				return eval1.good > eval2.good;
			}
		);
	}

	constexpr moves_t::const_iterator
	begin() const noexcept
	{
		return _moves.begin();
	}

	constexpr moves_t::const_iterator
	end() const noexcept
	{
		return _end;
	}

	constexpr size_t
	size() const noexcept
	{
		return std::distance(begin(), end());
	}

	constexpr score_t
	operator[](const move_t move) const noexcept
	{
		return _evals[move.index].score;
	}

protected:
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		return node.color() == white
			? generate<white_tag>(node, moves)
			: generate<black_tag>(node, moves);
	}

	template <typename color_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		moves = detail::generator<king_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<rook_queen_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<bishop_queen_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<knight_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<pawn_tag, color_tag>::template generate<moves_tag>(node, moves);
		return moves;
	}

private:
	moves_t _moves;
	moves_t::iterator _end;

	struct eval_t
	{
		float good;
		score_t score;
		std::uint8_t killer;
	};
	std::array<eval_t, 256> _evals;
};

}
