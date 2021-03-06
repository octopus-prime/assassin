/*
 * analyser.hpp
 *
 *  Created on: 27.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "attacker.hpp"

namespace chess {
namespace detail {

template <typename color_tag>
struct pawn_analyser_t
{
	typedef std::uint8_t properties_t;
	typedef std::array<std::array<properties_t, 64>, 6> entries_t; // [rank][index]

	pawn_analyser_t(const node_t& node) noexcept
	{
		for (std::size_t i = 0; i < 8; ++i)
		{
			constexpr board_t file = A2 | A3 | A4 | A5 | A6 | A7;
			const board_t mask = file << i;
			indexes_color[i] = _pext_u64(node.occupy<pawn_tag, color_tag>(), mask);
			indexes_other[i] = _pext_u64(node.occupy<pawn_tag, other_tag>(), mask);
		}

		backwards = make_backwards(node);
	}

	static board_t make_backwards(const node_t& node) noexcept;

	std::pair<properties_t,properties_t> // color,other
	operator()(const square_t square) const noexcept
	{
		const auto rank = rank_of(square) - 1; // 0...5
		const auto file = file_of(square); // 0...7

		properties_t properties_color = properties_color_0[rank][indexes_color[file]];
		properties_t properties_other = properties_other_0[rank][indexes_other[file]];

		if (file > 0)
		{
			properties_color |= properties_color_1[rank][indexes_color[file - 1]];
			properties_other |= properties_other_1[rank][indexes_other[file - 1]];
		}
		if (file < 7)
		{
			properties_color |= properties_color_1[rank][indexes_color[file + 1]];
			properties_other |= properties_other_1[rank][indexes_other[file + 1]];
		}

		properties_color |= bool(backwards & board_of(square)) << backwarded;
		return std::make_pair(properties_color, properties_other);
	}

	static constexpr bool
	is_isolated(const std::pair<properties_t,properties_t>& properties) noexcept
	{
		return !(properties.first & (1 << neighbour)); // no own pawns in file +/- 1
	}

	static constexpr bool
	is_doubled(const std::pair<properties_t,properties_t>& properties) noexcept
	{
		return properties.first & (1 << doubled); // more than 1 own pawn on file
	}

	static constexpr bool
	is_passed(const std::pair<properties_t,properties_t>& properties) noexcept
	{
		return !((properties.first | properties.second) & (1 << blocked)); // not blocked by own or other pawns
	}

	static constexpr bool
	is_supported(const std::pair<properties_t,properties_t>& properties) noexcept
	{
		return properties.first & (1 << supported); // own neighbour pawn on same rank or 1 rank behind
	}

	static constexpr bool
	is_backwarded(const std::pair<properties_t,properties_t>& properties) noexcept
	{
		return properties.first & (1 << backwarded);
	}

private:
	typedef typename color_traits<color_tag>::other other_tag;

	enum property : std::uint8_t
	{
		neighbour,
		doubled,
		blocked,
		supported,
		backwarded
	};

	std::array<size_t, 8> indexes_color;
	std::array<size_t, 8> indexes_other;
	board_t backwards;

	static const entries_t properties_color_0; // this file
	static const entries_t properties_other_0; // this file
	static const entries_t properties_color_1; // file +/- 1
	static const entries_t properties_other_1; // file +/- 1

	static entries_t make_color_0() noexcept;
	static entries_t make_color_1() noexcept;
	static entries_t make_other_0() noexcept;
	static entries_t make_other_1() noexcept;
};

template <>
inline board_t
pawn_analyser_t<white_tag>::make_backwards(const node_t& node) noexcept
{
	board_t spans = attacker<pawn_tag, white_tag>::attack(node.occupy<pawn_tag, white_tag>());
	for (std::size_t i = 0; i < 3; ++i)
		spans |= spans << (8 << i);

	board_t area = attacker<pawn_tag, black_tag>::attack(node.occupy<pawn_tag, black_tag>()) & ~spans;
	for (std::size_t i = 0; i < 3; ++i)
		area |= area >> (8 << i);

	return area & node.occupy<pawn_tag, white_tag>();
}

template <>
inline board_t
pawn_analyser_t<black_tag>::make_backwards(const node_t& node) noexcept
{
	board_t spans = attacker<pawn_tag, black_tag>::attack(node.occupy<pawn_tag, black_tag>());
	for (std::size_t i = 0; i < 3; ++i)
		spans |= spans >> (8 << i);

	board_t area = attacker<pawn_tag, white_tag>::attack(node.occupy<pawn_tag, white_tag>()) & ~spans;
	for (std::size_t i = 0; i < 3; ++i)
		area |= area << (8 << i);

	return area & node.occupy<pawn_tag, black_tag>();
}

}
}
