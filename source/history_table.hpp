/*
 * history_table.hpp
 *
 *  Created on: 14.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include <array>
#include <atomic>

namespace chess {

class history_table_t
{
public:
	constexpr
	history_table_t() noexcept
	:
		_entries()
	{
	}

	inline size_t
	operator()(const node_t& node, const move_t move) const noexcept
	{
		return _entries[node.color() == white][move.from][move.to];
	}

	inline void
	put(const node_t& node, const move_t move) noexcept
	{
		if (node[move.to] || move.promotion)
			return;
		++_entries[node.color() == white][move.from][move.to];
	}

	inline void
	clear() noexcept
	{
		for (auto& temp1 : _entries)
			for (auto& temp2 : temp1)
				std::fill(temp2.begin(), temp2.end(), 0);
	}

private:
	std::array<std::array<std::array<std::atomic_int_fast64_t, 64>, 64>, 2> _entries;
};

}
