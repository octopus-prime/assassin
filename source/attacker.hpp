/*
 * attacker.hpp
 *
 *  Created on: 27.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "board.hpp"
#include "tags.hpp"

namespace chess {
namespace detail {

struct filler
{
	static constexpr board4_t
	left(const board4_t board, const board4_t shift, const board4_t mask) noexcept
	{
		return (board << shift) & mask;
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t shift, const board4_t mask) noexcept
	{
		return (board >> shift) & mask;
	}

	static constexpr board4_t
	left(board4_t board, const board4_t shift, const board4_t mask, board4_t empty) noexcept
	{
		board4_t flood (board);
		empty &= mask;
		flood |= board = (board << shift) & empty;
		flood |= board = (board << shift) & empty;
		flood |= board = (board << shift) & empty;
		flood |= board = (board << shift) & empty;
		flood |= board = (board << shift) & empty;
		flood |=         (board << shift) & empty;
		return           (flood << shift) & mask;
	}

	static constexpr board4_t
	right(board4_t board, const board4_t shift, const board4_t mask, board4_t empty) noexcept
	{
		board4_t flood (board);
		empty &= mask;
		flood |= board = (board >> shift) & empty;
		flood |= board = (board >> shift) & empty;
		flood |= board = (board >> shift) & empty;
		flood |= board = (board >> shift) & empty;
		flood |= board = (board >> shift) & empty;
		flood |=         (board >> shift) & empty;
		return           (flood >> shift) & mask;
	}
};

template <typename piece_tag, typename color_tag>
struct attacker;

template <typename color_tag>
struct attacker<king_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = left(in) | right(in);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh, ~0UL, ~Fa};
		return filler::left(board, shift, mask);
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa, ~0UL, ~Fh};
		return filler::right(board, shift, mask);
	}

	static constexpr board4_t shift {1, 7, 8, 9};
};

template <typename color_tag>
struct attacker<rook_queen_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board, const board_t occupy) noexcept
	{
		const board4_t empty = make_board4(~occupy);
		const board4_t in = make_board4(board);
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL};
		return filler::left(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL};
		return filler::right(board, shift, mask, empty);
	}

	static constexpr board4_t shift {1, 8};
};

template <typename color_tag>
struct attacker<bishop_queen_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board, const board_t occupy) noexcept
	{
		const board4_t empty = make_board4(~occupy);
		const board4_t in = make_board4(board);
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		return filler::left(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		return filler::right(board, shift, mask, empty);
	}

	static constexpr board4_t shift {7, 9};
};

template <typename color_tag>
struct attacker<sliding_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t rooks, const board_t bishops, const board_t occupy) noexcept
	{
		const board4_t empty = make_board4(~occupy);
		const board4_t in {rooks, rooks, bishops, bishops};
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL, ~Fh, ~Fa};
		return filler::left(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL, ~Fa, ~Fh};
		return filler::right(board, shift, mask, empty);
	}

	static constexpr board4_t shift {1, 8, 7, 9};
};

template <typename color_tag>
struct attacker<knight_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = left(in) | right(in);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~(Fg | Fh), ~(Fa | Fb), ~Fh, ~Fa};
		return filler::left(board, shift, mask);
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~(Fa | Fb), ~(Fg | Fh), ~Fa, ~Fh};
		return filler::right(board, shift, mask);
	}

	static constexpr board4_t shift {6, 10, 15, 17};
};

template <>
struct attacker<pawn_tag, white_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = left(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		constexpr board4_t shift {7, 9};
		return filler::left(board, shift, mask);
	}
};

template <>
struct attacker<pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = right(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		constexpr board4_t shift {7, 9};
		return filler::right(board, shift, mask);
	}
};

}
}

//#include "attacker/filler.hpp"
//#include "attacker/attacker.hpp"
//#include "attacker/attacker_king.hpp"
//#include "attacker/attacker_rook.hpp"
//#include "attacker/attacker_bishop.hpp"
//#include "attacker/attacker_knight.hpp"
//#include "attacker/attacker_pawn.hpp"
//#include "attacker/attacker_sliding.hpp"

