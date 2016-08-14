/*
 * transposition_table.hpp
 *
 *  Created on: 14.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
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
public:
	inline transposition_table_t(const std::size_t size)
	{
		_entries[0].resize(size / sizeof(transposition_entry_t) / 2 + 13, transposition_entry_t());
		_entries[1].resize(size / sizeof(transposition_entry_t) / 2 + 13, transposition_entry_t());
	}


	inline void clear() noexcept
	{
		std::fill(_entries[0].begin(), _entries[0].end(), transposition_entry_t());
		std::fill(_entries[1].begin(), _entries[1].end(), transposition_entry_t());
	}

	inline void put(const node_t& node, const move_t move, const score_t score, const transposition_entry_t::flag_t flag, const std::uint8_t depth) noexcept
	{
		transposition_entry_t& entry = operator[](node);
		if (entry.depth() < depth)
			entry = transposition_entry_t(node.hash(), move, score, flag, depth);
	}

	inline transposition_entry_t& operator[](const node_t& node) noexcept
	{
		return _entries[node.color() == white][node.hash() % _entries[node.color() == white].size()];
	}

	inline const transposition_entry_t& operator[](const node_t& node) const noexcept
	{
		return _entries[node.color() == white][node.hash() % _entries[node.color() == white].size()];
	}

private:
	std::array<std::vector<transposition_entry_t>, 2> _entries;
};

}
