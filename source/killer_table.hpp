/*
 * killer_table.hpp
 *
 *  Created on: 19.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include <array>
#include <atomic>

namespace chess {

class killer_table_t
{
public:
	typedef std::array<std::atomic<move_t>, 2> entry_t;

	void put(const node_t& node, const move_t move, const std::uint8_t height) noexcept
	{
		if (node[move.to])
			return;
		entry_t& entry = _entries[height];
		if (entry[0] != move && entry[1] != move)
		{
			const move_t temp = entry[0];
			entry[1] = temp;
			entry[0] = move;
		}
	}

	const entry_t& operator[](const std::uint8_t height) const noexcept
	{
		return _entries[height];
	}

private:
	std::array<entry_t, 256> _entries;
};

}
