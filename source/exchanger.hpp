/*
 * exchanger.hpp
 *
 *  Created on: 18.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"

namespace chess {

struct exchanger
{
	static score_t
	evaluate(const node_t& node, const move_t move) noexcept;
};

}
