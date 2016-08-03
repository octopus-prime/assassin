/*
 * attacker_king.hpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

template <typename color_tag>
struct attacker<king_tag, color_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
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

}
