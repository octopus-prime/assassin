/*
 * evaluator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"

namespace chess {

struct evaluator
{
	static score_t
	evaluate(const node_t& node) noexcept;

private:
	template <typename feature_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept;
};

}
