/*
 * attack_generator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "attacker.hpp"

namespace chess {
namespace detail {

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

struct attack_generator
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
