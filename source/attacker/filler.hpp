/*
 * filler.hpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

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

}
