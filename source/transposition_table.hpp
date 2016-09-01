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

class transposition_entry_t
{
public:
	enum flag_t : std::uint8_t
	{
		UNKNOWN,
		UPPER,
		LOWER,
		EXACT
//		EGTB
	};

public:
	constexpr transposition_entry_t() noexcept
	:
		_hash(),
		_move(),
		_score(),
		_depth(),
		_flag()
	{
	}

	constexpr transposition_entry_t(const hash_t hash, const move_t move, const score_t score, const flag_t flag, const std::uint8_t depth) noexcept
	:
		_hash(hash),
		_move(move),
		_score(score),
		_depth(depth),
		_flag(flag)
	{
	}

	constexpr hash_t hash() const noexcept
	{
		return _hash;
	}

	constexpr move_t move() const noexcept
	{
		return _move;
	}

	constexpr score_t score() const noexcept
	{
		return _score;
	}

	constexpr flag_t flag() const noexcept
	{
		return _flag;
	}

	constexpr std::uint8_t depth() const noexcept
	{
		return _depth;
	}

private:
	hash_t			_hash;
	move_t			_move;
	score_t			_score;
	std::uint8_t	_depth;
	flag_t			_flag;
};

class transposition_table_t
{
	typedef std::array<transposition_entry_t, 4> cluster_t;

	static_assert(sizeof(cluster_t) == 64, "Bad cluster size");

public:
	inline transposition_table_t(const std::size_t size)
	:
		_entries(size)// / sizeof(cluster_t) + 13)
	{
	}


	inline void clear() noexcept
	{
		std::fill(_entries.begin(), _entries.end(), cluster_t());
	}

	inline void put(const node_t& node, const move_t move, const score_t score, const transposition_entry_t::flag_t flag, const std::uint8_t depth) noexcept
	{
//		transposition_entry_t& entry = operator[](node);
//		if (entry.depth() < depth)
//			entry = transposition_entry_t(node.hash(), move, score, flag, depth);

		cluster_t& cluster = get(node);
		transposition_entry_t* replace = &cluster.front();
		for (transposition_entry_t& entry : cluster)
		{
			if (entry.hash() == node.hash())
			{
				if (move == move_t {})
					entry = transposition_entry_t(node.hash(), entry.move(), score, flag, depth);
				else
					entry = transposition_entry_t(node.hash(), move, score, flag, depth);
				return;
			}

			if (entry.depth() < replace->depth())
				replace = &entry;
		}

		if (depth >= replace->depth())
			*replace = transposition_entry_t(node.hash(), move, score, flag, depth);
	}

//	inline transposition_entry_t& operator[](const node_t& node) noexcept
//	{
//		return _entries[node.hash() % _entries.size()];
//	}

	inline const transposition_entry_t* const operator[](const node_t& node) const noexcept
	{
		const cluster_t& cluster = get(node);
		for (const transposition_entry_t& entry : cluster)
			if (entry.hash() == node.hash())
				return &entry;
		return nullptr;
	}

protected:
	cluster_t& get(const node_t& node) noexcept
	{
//		return _entries[node.hash() % _entries.size()];
		return _entries[node.hash() & (_entries.size() - 1)];
	}

	const cluster_t& get(const node_t& node) const noexcept
	{
//		return _entries[node.hash() % _entries.size()];
		return _entries[node.hash() & (_entries.size() - 1)];
	}

private:
	std::vector<cluster_t> _entries;
};

}
