/*
 * hash.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "hash.hpp"
#include <random>
#include <algorithm>

namespace chess {

static inline hash_t
random()
{
	static std::mt19937_64 generator;
	static std::uniform_int_distribution<hash_t> distribution;

	return distribution(generator);
}

static std::array<std::array<hash_t, 64>, 13>
create_piece_square_hash()
{
	std::array<std::array<hash_t, 64>, 13> hash;

	std::fill_n(hash[no_piece].begin(), 64, 0ULL);
	for (piece_t piece = K; piece <= p; ++piece)
		std::generate_n(hash[piece].begin(), 64, random);

	return std::move(hash);
}

static std::array<hash_t, 64>
create_en_passant_hash()
{
	std::array<hash_t, 64> hash;

	std::fill_n(hash.begin() + 0, 16, 0);
	std::generate_n(hash.begin() + 16, 8, random);
	std::fill_n(hash.begin() + 24, 16, 0);
	std::generate_n(hash.begin() + 40, 8, random);
	std::fill_n(hash.begin() + 48, 16, 0);

	return std::move(hash);
}

static std::array<hash_t, 16>
create_castle_hash()
{
	std::array<hash_t, 16> hash;

	std::generate_n(hash.begin(), 16, random);

	return std::move(hash);
}

const std::array<std::array<hash_t, 64>, 13>
piece_square_hash = create_piece_square_hash();

const std::array<hash_t, 64>
en_passant_hash = create_en_passant_hash();

const std::array<hash_t, 16>
castle_hash = create_castle_hash();

const hash_t
color_hash = random();

}
