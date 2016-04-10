/*
 * test_square.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

#include "square.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>
#include <array>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_square)

constexpr std::array<square_rank_t, 64> ranks
{
	0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 1, 1, 1, 1, 1, 1,
	2, 2, 2, 2, 2, 2, 2, 2,
	3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7
};

constexpr std::array<square_file_t, 64> files
{
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7
};

BOOST_DATA_TEST_CASE(test_of, data::xrange(64) ^ files ^ ranks, square, file, rank)
{
	BOOST_CHECK_EQUAL(file_of(square), file);
	BOOST_CHECK_EQUAL(rank_of(square), rank);
	BOOST_CHECK_EQUAL(square_of(file, rank), square);
}

BOOST_AUTO_TEST_SUITE_END()

}
