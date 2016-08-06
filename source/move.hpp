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
};

typedef std::array<move_t, 100> moves_t;

}
