/*
 * test_searcher.cpp
 *
 *  Created on: 20.08.2016
 *      Author: mike
 */

#include "searcher.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace data = boost::unit_test::data;

namespace chess {

BOOST_AUTO_TEST_SUITE(test_searcher)

const std::initializer_list<node_t> nodes
{
	node_t("k7/7R/8/8/8/8/8/1R5K b - - 0 1"), // patt
	node_t("k7/7R/8/8/8/8/8/1R5K w - - 0 1"), // mate+
	node_t("k7/7R/8/8/8/8/8/2R4K b - - 0 1"), // mate-
	node_t("k7/7R/8/8/8/8/8/2R4K b - - 49 1") // 50 moves
};

const std::initializer_list<score_t> scores
{
	0,
	+29997,
	-29996,
	0
};

const auto output = [](const std::uint_fast8_t, const std::uint_fast8_t, const score_t, const auto, const std::size_t, const std::string&)
{
};

BOOST_DATA_TEST_CASE(test, nodes ^ scores, node, score)
{
	transposition_table_t table(100);
	searcher search(table, output);
	BOOST_CHECK_EQUAL(search(node, 10), score);
}

BOOST_AUTO_TEST_SUITE_END()

}
