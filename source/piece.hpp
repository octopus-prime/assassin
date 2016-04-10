/*
 * piece.hpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#pragma once

#include <cstdint>

namespace chess {

typedef std::uint_fast8_t piece_t;

enum piece : piece_t
{
	no_piece,
	K, Q, R, B, N, P,
	k, q, r, b, n, p
};

}
