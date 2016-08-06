/*
 * io.hpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#pragma once

#include "board.hpp"
#include "move.hpp"
#include "node.hpp"
#include <bitset>
#include <iostream>

namespace chess {

}

namespace std {

ostream&
operator<<(ostream& stream, const bitset<64>& board);

ostream&
operator<<(ostream& stream, const chess::move_t& move);

ostream&
operator<<(ostream& stream, const chess::node_t& node);

}