/*
 * hash.hpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#pragma once

#include "piece.hpp"
#include "square.hpp"
#include "castle.hpp"
#include <array>

namespace chess {

typedef std::uint64_t hash_t;

extern const std::array<std::array<hash_t, 64>, 13> piece_square_hash;
extern const std::array<hash_t, 64> en_passant_hash;
extern const std::array<hash_t, 16> castle_hash;
extern const hash_t color_hash;

}
