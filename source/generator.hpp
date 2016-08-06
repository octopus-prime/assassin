/*
 * generator.hpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#pragma once

#include "board.hpp"
#include "move.hpp"
#include "node.hpp"
#include "attacker.hpp"

namespace chess {

template <typename piece_tag, typename color_tag>
struct generator
{
	static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<piece_tag, color_tag>::attack(node, node.occupy<piece_tag, color_tag>());
	}

	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<piece_tag, color_tag>()))
			for (const auto to : bsf(attacker<piece_tag, color_tag>::attack(node, board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<king_tag, color_tag>
{
	static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<king_tag, color_tag>::attack(node, node.occupy<king_tag, color_tag>());
	}

	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		const square_t from = node.king<color_tag>();
		for (const auto to : bsf(attacker<king_tag, color_tag>::attack(node, board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
			*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<sliding_tag, color_tag>
{
	static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<sliding_tag, color_tag>::attack(node, node.occupy<rook_tag, color_tag>(), node.occupy<bishop_tag, color_tag>());
	}
};

template <typename color_tag>
struct generator<pawn_tag, color_tag>
{
	static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<pawn_tag, color_tag>::attack(node, node.occupy<pawn_tag, color_tag>());
	}

	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept;
//	{
//		const square_t from = node.king<color_tag>();
//		for (const auto to : bsf(attacker<king_tag, color_tag>::attack(node, board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
//			*moves++ = move_t {from, to};
//		return moves;
//	}
};

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, white_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, black_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	return moves;
}


struct attack_generator // todo: move to own file
{
	template <typename color_tag>
	static board_t
	generate(const node_t& node) noexcept
	{
		board_t board = 0;
		board |= generator<king_tag, color_tag>::generate(node);
		board |= generator<sliding_tag, color_tag>::generate(node);
		board |= generator<knight_tag, color_tag>::generate(node);
		board |= generator<pawn_tag, color_tag>::generate(node);
		return board;
	}
};

}
