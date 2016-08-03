/*
 * attacker.hpp
 *
 *  Created on: 27.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "node.hpp"

namespace chess {

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

template <>
struct masker<active_tag, white_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return node.occupy<black_tag>();
	}
};

template <>
struct masker<active_tag, black_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return node.occupy<white_tag>();
	}
};

}

#include "attacker/filler.hpp"
#include "attacker/attacker.hpp"
#include "attacker/attacker_king.hpp"
#include "attacker/attacker_rook.hpp"
#include "attacker/attacker_bishop.hpp"
#include "attacker/attacker_knight.hpp"
#include "attacker/attacker_pawn.hpp"
/*
template <typename moves_tag, typename piece_tag, typename color_tag>
struct attacker;

template <typename moves_tag, typename color_tag>
struct attacker<moves_tag, king_tag, color_tag>
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
//		constexpr board4_t shift {1, 7, 8, 9};
		return (board << shift) & mask;
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa, ~0UL, ~Fh};
//		constexpr board4_t shift {1, 7, 8, 9};
		return (board >> shift) & mask;
	}

	static constexpr board4_t shift {1, 7, 8, 9};
};

template <typename moves_tag, typename color_tag>
struct attacker<moves_tag, rook_tag, color_tag>
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
	left(board4_t board, board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL};
		constexpr board4_t shift {1, 8};

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
	right(board4_t board, board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL};
		constexpr board4_t shift {1, 8};

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

template <typename moves_tag, typename color_tag>
struct attacker<moves_tag, bishop_tag, color_tag>
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
	left(board4_t board, board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		constexpr board4_t shift {7, 9};

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
	right(board4_t board, board4_t empty) noexcept
	{
		constexpr board4_t mask {~Fa, ~Fh};
		constexpr board4_t shift {7, 9};

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

template <typename moves_tag, typename color_tag>
struct attacker<moves_tag, knight_tag, color_tag>
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
		constexpr board4_t mask {~(Fg | Fh), ~(Fa | Fb), ~Fh, ~Fa};
		constexpr board4_t shift {6, 10, 15, 17};
		return (board << shift) & mask;
	}

	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~(Fa | Fb), ~(Fg | Fh), ~Fa, ~Fh};
		constexpr board4_t shift {6, 10, 15, 17};
		return (board >> shift) & mask;
	}
};

template <typename moves_tag>
struct attacker<moves_tag, pawn_tag, white_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = left(in);
		return out[1] | out[3] | ((out[0] | out[2]) & node.board_occupy<black_tag>());
	}

private:
	static constexpr board4_t
	left(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fa, ~0UL, ~Fh, R2};
		constexpr board4_t shift {7, 8, 9, 16};
		return (board & mask) << shift;
	}
};

template <>
struct attacker<active_tag, pawn_tag, white_tag>
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
	}
};

template <>
struct attacker<passive_tag, pawn_tag, white_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t empty = make_board4(~node.board_occupy());
		const board4_t in = make_board4(board);
		const board4_t out = left(in, empty);
		return out[0] | (out[1] & out[2]);
	}

private:
	static constexpr board4_t
	left(const board4_t board, const board4_t empty) noexcept
	{
		constexpr board4_t shift1 {8, 8, 16};
		constexpr board4_t shift2 {0, 8, 0};
		constexpr board4_t mask {~0UL, R2, R2};
		return (((board & mask) << shift1) & empty) << shift2;
	}
};

template <typename moves_tag>
struct attacker<moves_tag, pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = right(in);
		return out[1] | out[3] | ((out[0] | out[2]) & node.board_occupy<white_tag>());
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~0UL, ~Fa, R7};
		constexpr board4_t shift {7, 8, 9, 16};
		return (board & mask) >> shift;
	}
};

template <>
struct attacker<active_tag, pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = right(in);
		return (out[0] | out[1]);// &  masker<active_tag, black_tag>::mask(node);;// node.board_occupy<white_tag>();
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~Fh, ~Fa};
		constexpr board4_t shift {7, 9};
		return (board & mask) >> shift;
	}
};

template <>
struct attacker<passive_tag, pawn_tag, black_tag>
{
	static constexpr board_t
	attack(const node_t& node, const board_t board) noexcept
	{
		const board4_t in = make_board4(board);
		const board4_t out = right(in);
//		return (out[0] | out[1]) & masker<passive_tag, black_tag>::mask(node);
		return out[0] & masker<passive_tag, black_tag>::mask(node);
	}

private:
	static constexpr board4_t
	right(const board4_t board) noexcept
	{
		constexpr board4_t mask {~0UL};
		constexpr board4_t shift {8};
		return (board & mask) >> shift;
	}
};

}
*/
