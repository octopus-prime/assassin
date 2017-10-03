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

const auto output = [](const std::uint_fast8_t i, const std::uint_fast8_t,
                       const score_t, const auto, const std::size_t,
                       const std::string&) {
};

const node_t n0("2k5/pp2n2Q/8/P2p4/6q1/P1p5/2P2PKP/5R2 w - - 0 1");
const node_t n1(n0, {g2, h1});  // 1
const node_t n2(n1, {g4, f3});
const node_t n3(n2, {h1, g1});
const node_t n4(n3, {f3, g4});
const node_t n5(n4, {g1, h1});  // 2
const node_t n6(n5, {g4, f3});
const node_t n7(n6, {h1, g1});
const node_t n8(n7, {f3, g4});
const node_t n9(n8, {g1, h1});  // 3

node_t const node0("5k2/p4p2/1p2q3/5RQp/6n1/4P3/PP6/6K1 w - - 0 34");
node_t const node1(node0, {g5, d8});
node_t const node2(node1, {f8, g7});  // 35. Qd8+ Kg7
node_t const node3(node2, {d8, g5});
node_t const node4(node3, {g7, f8});  // 36. Qg5+ Kf8 (second time)
node_t const node5(node4, {g5, d8});
node_t const node6(node5, {f8, g7});  // 37. Qd8+ Kg7
node_t const node7(node6, {d8, g5});
node_t const node8(node7, {g7, f8});  // 38. Qg5+ Kf8 (third time)

const std::initializer_list<node_t> nodes{
    node_t("k7/7R/8/8/8/8/8/1R5K b - - 0 1"),   // patt
    node_t("k7/7R/8/8/8/8/8/1R5K w - - 0 1"),   // mate+
    node_t("k7/7R/8/8/8/8/8/2R4K b - - 0 1"),   // mate-
    node_t("k7/7R/8/8/8/8/8/2R4K b - - 99 1"),  // 50 moves
    n9,                                         // 3fold repetition
    node8,                                      // 3fold repetition
};

const std::initializer_list<score_t> scores{0, +29997, -29996, 0, 0, 0};

BOOST_DATA_TEST_CASE(test, nodes ^ scores, node, score) {
  transposition_table_t table(100);
  searcher search(table, output);
  BOOST_CHECK_EQUAL(search(node, 10), score);
}

BOOST_AUTO_TEST_SUITE_END()

}  // namespace chess
