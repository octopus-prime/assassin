/*
 * move_generator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include "history_table.hpp"
#include "killer_table.hpp"

namespace chess {

typedef std::array<move_t, 256> moves_t;

template <typename moves_tag>
class move_generator
{
	struct eval_t
	{
		float good;
		score_t score;
		std::uint8_t killer;
	};

public:
	move_generator(const node_t& node);
	move_generator(const node_t& node, const history_table_t& h_table, const history_table_t& b_table, const killer_table_t::entry_t& k_entry1, const killer_table_t::entry_t& k_entry2);

	constexpr moves_t::const_iterator
	begin() const noexcept
	{
		return _moves.begin();
	}

	constexpr moves_t::const_iterator
	end() const noexcept
	{
		return _end;
	}

	constexpr size_t
	size() const noexcept
	{
		return std::distance(begin(), end());
	}

	constexpr score_t
	operator[](const move_t move) const noexcept
	{
		return _evals[move.index].score;
	}

protected:
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves);

	template <typename color_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves);

private:
	moves_t _moves;
	moves_t::iterator _end;
	std::array<eval_t, 256> _evals;
};

}
