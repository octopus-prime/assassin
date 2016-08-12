/*
 * move.hpp
 *
 *  Created on: 21.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "square.hpp"
#include <array>

namespace chess {

struct move_t
{
	square_t from;
	square_t to;
	std::uint8_t promotion;
};

constexpr bool
operator==(const move_t move1, const move_t move2) noexcept
{
	return move1.from == move2.from && move1.to == move2.to && move1.promotion == move2.promotion;
}

constexpr bool
operator!=(const move_t move1, const move_t move2) noexcept
{
	return !(move1 == move2);
}

typedef std::array<move_t, 256> moves_t;

}
