/*
 * attacker_sliding.hpp
 *
 *  Created on: 05.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

template <typename color_tag>
struct attacker<sliding_tag, color_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t rooks, const board_t bishops) noexcept
	{
		const board4_t empty = make_board4(~node.occupy());
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

}
