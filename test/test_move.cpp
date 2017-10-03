/*
 * test_move.cpp
 *
 *  Created on: 01.10.2017
 *      Author: mike
 */

#include "move.hpp"
#include "io.hpp"
#include <boost/test/unit_test.hpp>
#include <boost/test/data/test_case.hpp>

namespace chess {

static_assert(sizeof(move_t) == 4, "move_t size");

BOOST_AUTO_TEST_SUITE(test_move)

constexpr std::initializer_list<move_t> moves1{
  {d3, f5, move_t::no, 0},
  {d3, f5, move_t::no, 1}
};

BOOST_AUTO_TEST_SUITE(test_equal_to)

constexpr std::initializer_list<move_t> moves2{
  {d3, f5, move_t::no, 0},
  {d3, f5, move_t::no, 1}
};

BOOST_DATA_TEST_CASE(test, moves1 * moves2, move1, move2) {
  BOOST_CHECK_EQUAL(move1, move2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(test_not_equal_to)

constexpr std::initializer_list<move_t> moves2{
  {d3, f5, move_t::N, 0},
  {d3, e4, move_t::no, 0},
  {c2, f5, move_t::no, 0}
};

BOOST_DATA_TEST_CASE(test, moves1 * moves2, move1, move2) {
  BOOST_CHECK_NE(move1, move2);
}

BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE_END()

}  // namespace chess
