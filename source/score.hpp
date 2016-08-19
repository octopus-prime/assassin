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
//	0, +900, +500, +333, +325, +100,
//	0, +900, +500, +333, +325, +100

// todo: king scores should not be zero, to sort them last in capture lists...
	+30000, +900, +500, +333, +325, +100,
	+30000, +900, +500, +333, +325, +100
};

constexpr score_t
operator""_P(unsigned long long x) noexcept
{
	return x * score_of[P];
}

constexpr score_t
operator""_N(unsigned long long x) noexcept
{
	return x * score_of[N];
}

constexpr score_t
operator""_B(unsigned long long x) noexcept
{
	return x * score_of[B];
}

constexpr score_t
operator""_R(unsigned long long x) noexcept
{
	return x * score_of[R];
}

constexpr score_t
operator""_Q(unsigned long long x) noexcept
{
	return x * score_of[Q];
}

constexpr score_t
operator""_K(unsigned long long x) noexcept
{
	return x * score_of[K];
}

}
