/*
 * attacker_rook.hpp
 *
 *  Created on: 03.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

template <typename color_tag>
struct attacker<rook_tag, color_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t empty = make_board4(~node.board_occupy());
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

}
