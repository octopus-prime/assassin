/*
 * test_hash.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "hash.hpp"
#include <boost/test/unit_test.hpp>
#include <unordered_set>

namespace chess {

BOOST_AUTO_TEST_SUITE(test_hash)

BOOST_AUTO_TEST_CASE(test_hash_disjoined)
{
	std::unordered_set<hash_t> hashes;

	for (piece_t piece = K; piece <= p; ++piece)
		for (square_t square = a1; square <= h8; ++square)
			hashes.emplace(piece_square_hash[piece][square]);

	for (square_t square = a3; square <= h3; ++square)
		hashes.emplace(en_passant_hash[square]);
	for (square_t square = a6; square <= h6; ++square)
		hashes.emplace(en_passant_hash[square]);

	for (castle_t castle = 0; castle <= 15; ++castle)
		hashes.emplace(castle_hash[castle]);

//	hashes.emplace(color_hash);

	BOOST_CHECK_EQUAL(hashes.size(), 12 * 64 + 2 * 8 + 16);// + 1);
}

BOOST_AUTO_TEST_CASE(test_hash_zeroed)
{
	std::unordered_set<hash_t> hashes;

	for (square_t square = a1; square <= h8; ++square)
		hashes.emplace(piece_square_hash[no_piece][square]);

	for (square_t square = a1; square <= h2; ++square)
		hashes.emplace(en_passant_hash[square]);
	for (square_t square = a4; square <= h5; ++square)
		hashes.emplace(en_passant_hash[square]);
	for (square_t square = a7; square <= h8; ++square)
		hashes.emplace(en_passant_hash[square]);

	BOOST_CHECK_EQUAL(hashes.size(), 1);
}

BOOST_AUTO_TEST_SUITE_END()

}

