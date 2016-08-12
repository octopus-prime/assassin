/*
 * score.hpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#pragma once

#include <cstdint>

namespace chess {

typedef std::int16_t score_t;

static constexpr std::array<score_t, 13> score_of
{
	0,
	+30000, +900, +500, +333, +325, +100,
	+30000, +900, +500, +333, +325, +100
};

static constexpr std::array<score_t, 5> scoring
{
	0,
	score_of[N] - score_of[P],
	score_of[B] - score_of[P],
	score_of[R] - score_of[P],
	score_of[Q] - score_of[P]
};

}
