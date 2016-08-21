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

}

constexpr chess::score_t operator "" _P(const unsigned long long value) {return value * 100;}
constexpr chess::score_t operator "" _N(const unsigned long long value) {return value * 325;}
constexpr chess::score_t operator "" _B(const unsigned long long value) {return value * 333;}
constexpr chess::score_t operator "" _R(const unsigned long long value) {return value * 500;}
constexpr chess::score_t operator "" _Q(const unsigned long long value) {return value * 900;}
constexpr chess::score_t operator "" _K(const unsigned long long value) {return value * 30000;}

namespace chess {

constexpr std::array<score_t, 13> score_of
{
	0,
	1_K, 1_Q, 1_R, 1_B, 1_N, 1_P,
	1_K, 1_Q, 1_R, 1_B, 1_N, 1_P,
};

}
