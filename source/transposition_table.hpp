/*
 * transposition_table.hpp
 *
 *  Created on: 14.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include <vector>
#include <array>

namespace chess {

class transposition_table_t
{
	static constexpr auto line = 64; // size of cache line
	static constexpr auto bits = 64 - 16; // complete hash - hash part of entry

public:
	enum flag_t : std::uint8_t
	{
		UNKNOWN,
		UPPER,
		LOWER,
		EXACT
//		EGTB
	};

	struct entry_t
	{
		move_t			move;
		std::uint16_t	hash;
		score_t			score;
		flag_t			flag;
		std::uint8_t	depth;
	};

private:
	struct alignas(line) cluster_t
	{
		std::array<entry_t, line / sizeof(entry_t)> entries;
	};

	static_assert(sizeof(cluster_t) == line, "cluster size != cache line");

public:
	inline transposition_table_t(const std::size_t size)
	:
		_clusters(size / sizeof(cluster_t))// + 13)
	{
	}


	inline void clear() noexcept
	{
		std::fill(_clusters.begin(), _clusters.end(), cluster_t());
	}

	inline void put(const node_t& node, move_t move, const score_t score, const flag_t flag, const std::uint8_t depth) noexcept
	{
		const std::uint16_t hash = node.hash() >> bits;
		cluster_t& cluster = get(node);
		entry_t* replace = &cluster.entries.front();
		for (entry_t& entry : cluster.entries)
		{
			if (entry.hash == hash)
			{
				if (move == move_t {})
					move = entry.move;
				entry = entry_t {move, hash, score, flag, depth};
				return;
			}

			if (entry.depth < replace->depth)
				replace = &entry;
		}

		if (depth > replace->depth)
			*replace = entry_t {move, hash, score, flag, depth};
	}

	inline const entry_t* const operator[](const node_t& node) const noexcept
	{
		const std::uint16_t hash = node.hash() >> bits;
		const cluster_t& cluster = get(node);
		for (const entry_t& entry : cluster.entries)
			if (entry.hash == hash)
				return &entry;
		return nullptr;
	}

	inline void prefetch(const node_t& node) const noexcept
	{
		const cluster_t& cluster = get(node);
		_mm_prefetch(&cluster, _MM_HINT_T0);
	}

protected:
	inline cluster_t& get(const node_t& node) noexcept
	{
		constexpr std::size_t S = (1UL << bits) - 1;
		return _clusters[(node.hash() & S) % _clusters.size()];
	}

	inline const cluster_t& get(const node_t& node) const noexcept
	{
		constexpr std::size_t S = (1UL << bits) - 1;
		return _clusters[(node.hash() & S) % _clusters.size()];
	}

private:
	std::vector<cluster_t> _clusters;
};

}
