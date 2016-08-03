/*
 * attacker_pawn.hpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

template <>
struct attacker<pawn_tag, white_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = left(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		constexpr board4_t shift {7, 9};
		return (board & mask) << shift;
//		return filler::left(board, shift, mask);
	}
};

template <>
struct attacker<pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = right(in);
		return out[0] | out[1];
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		constexpr board4_t shift {7, 9};
		return (board & mask) >> shift;
//		return filler::right(board, shift, mask);
	}
};

}
