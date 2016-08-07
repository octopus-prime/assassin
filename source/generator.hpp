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
namespace detail {

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
		const square_t from = node.king<color_tag>();
		for (const auto to : bsf(attacker<king_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
			*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<pawn_tag, color_tag>
{
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

template <typename piece_tag, typename color_tag>
struct attack_generator
{
	constexpr static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<piece_tag, color_tag>::attack(node.occupy<piece_tag, color_tag>());
	}
};

template <typename color_tag>
struct attack_generator<sliding_tag, color_tag>
{
	constexpr static board_t
	generate(const node_t& node) noexcept
	{
		return attacker<sliding_tag, color_tag>::attack(node.occupy<rook_queen_tag, color_tag>(), node.occupy<bishop_queen_tag, color_tag>(), node.occupy());
	}
};

}

struct attack_generator // todo: move to own file
{
	template <typename color_tag>
	constexpr static board_t
	generate(const node_t& node) noexcept
	{
		board_t board = 0;
		board |= detail::attack_generator<king_tag, color_tag>::generate(node);
		board |= detail::attack_generator<sliding_tag, color_tag>::generate(node);
		board |= detail::attack_generator<knight_tag, color_tag>::generate(node);
		board |= detail::attack_generator<pawn_tag, color_tag>::generate(node);
		return board;
	}
};

}
