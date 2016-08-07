/*
 * attacker.hpp
 *
 *  Created on: 27.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "board.hpp"
#include "tags.hpp"
#include <immintrin.h>

namespace chess {
namespace detail {

typedef __v4du board4_t;

constexpr inline board4_t
board4_of(const board_t board) noexcept
{
	return board4_t {board, board, board, board};
}

struct left_tag;
struct right_tag;

template <typename shift_tag>
struct filler;

template <>
struct filler<left_tag>
{
	static constexpr board4_t
	fill(const board4_t board, const board4_t shift, const board4_t mask) noexcept
	{
		return (board << shift) & mask;
	}

	static constexpr board4_t
	fill(board4_t board, const board4_t shift, const board4_t mask, board4_t empty) noexcept
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
};

template <>
struct filler<right_tag>
{
	static constexpr board4_t
	fill(const board4_t board, const board4_t shift, const board4_t mask) noexcept
	{
		return (board >> shift) & mask;
	}

	static constexpr board4_t
	fill(board4_t board, const board4_t shift, const board4_t mask, board4_t empty) noexcept
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
		const board4_t in = board4_of(board);
		const board4_t out = left(in) | right(in);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh, ~0UL, ~Fa};
		return filler<left_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa, ~0UL, ~Fh};
		return filler<right_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t shift {1, 7, 8, 9};
};

template <typename color_tag>
struct attacker<rook_queen_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board, const board_t occupy) noexcept
	{
		const board4_t empty = board4_of(~occupy);
		const board4_t in = board4_of(board);
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL};
		return filler<left_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL};
		return filler<right_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t shift {1, 8};
};

template <typename color_tag>
struct attacker<bishop_queen_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board, const board_t occupy) noexcept
	{
		const board4_t empty = board4_of(~occupy);
		const board4_t in = board4_of(board);
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		return filler<left_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		return filler<right_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t shift {7, 9};
};

template <typename color_tag>
struct attacker<sliding_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t rooks, const board_t bishops, const board_t occupy) noexcept
	{
		const board4_t empty = board4_of(~occupy);
		const board4_t in {rooks, rooks, bishops, bishops};
		const board4_t out = left(in, empty) | right(in, empty);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL, ~Fh, ~Fa};
		return filler<left_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t
	right(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL, ~Fa, ~Fh};
		return filler<right_tag>::fill(board, shift, mask, empty);
	}

	static constexpr board4_t shift {1, 8, 7, 9};
};

template <typename color_tag>
struct attacker<knight_tag, color_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = board4_of(board);
		const board4_t out = left(in) | right(in);
		return out[0] | out[1] | out[2] | out[3];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~(Fg | Fh), ~(Fa | Fb), ~Fh, ~Fa};
		return filler<left_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~(Fa | Fb), ~(Fg | Fh), ~Fa, ~Fh};
		return filler<right_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t shift {6, 10, 15, 17};
};

template <>
struct attacker<pawn_tag, white_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = board4_of(board);
		const board4_t out = left(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		return filler<left_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t shift {7, 9};
};

template <>
struct attacker<pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const board_t board) noexcept
	{
		const board4_t in = board4_of(board);
		const board4_t out = right(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		return filler<right_tag>::fill(board, shift, mask);
	}

	static constexpr board4_t shift {7, 9};
};

}
}
