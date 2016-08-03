/*
 * node.hpp
 *
 *  Created on: 21.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "square.hpp"
#include "color.hpp"
#include "board.hpp"
#include "hash.hpp"
#include <array>

namespace chess {

struct white_tag;
struct black_tag;

struct king_tag;
struct rook_tag;
struct bishop_tag;
struct knight_tag;
struct pawn_tag;

struct all_tag;
struct active_tag;
struct passive_tag;

struct node_t
{
	board_t occupy_white;
	board_t occupy_black;
	board_t occupy_rook_queen;
	board_t occupy_bishop_queen;
	board_t occupy_knight;
	board_t occupy_pawn;
	std::array<piece_t, 64> pieces;
	square_t king_white;
	square_t king_black;
	color_t color;
	board_t attack_white;
	board_t attack_black;

//	template <typename piece_tag>
//	constexpr board_t board_piece() const noexcept;

	template <typename piece_or_color_tag>
	constexpr board_t occupy() const noexcept;

	template <typename piece_or_color_tag1, typename piece_or_color_tag2>
	constexpr board_t occupy() const noexcept
	{
		return occupy<piece_or_color_tag1>() & occupy<piece_or_color_tag2>();
	}

	constexpr board_t occupy() const noexcept
	{
		return occupy_white | occupy_black;
	}

	template <typename color_tag>
	constexpr square_t king() const noexcept;

	template <typename color_tag>
	constexpr board_t attack() const noexcept;
};

template <>
constexpr board_t
node_t::occupy<white_tag>() const noexcept
{
	return occupy_white;
}

template <>
constexpr board_t
node_t::occupy<black_tag>() const noexcept
{
	return occupy_black;
}

template <>
constexpr board_t
node_t::occupy<king_tag>() const noexcept
{
	return board_of(king_white) | board_of(king_black);
}

template <>
constexpr board_t
node_t::occupy<rook_tag>() const noexcept
{
	return occupy_rook_queen;
}

template <>
constexpr board_t
node_t::occupy<bishop_tag>() const noexcept
{
	return occupy_bishop_queen;
}

template <>
constexpr board_t
node_t::occupy<knight_tag>() const noexcept
{
	return occupy_knight;
}

template <>
constexpr board_t
node_t::occupy<pawn_tag>() const noexcept
{
	return occupy_pawn;
}

template <>
constexpr square_t
node_t::king<white_tag>() const noexcept
{
	return king_white;
}

template <>
constexpr square_t
node_t::king<black_tag>() const noexcept
{
	return king_black;
}

template <>
constexpr board_t
node_t::attack<white_tag>() const noexcept
{
	return attack_white;
}

template <>
constexpr board_t
node_t::attack<black_tag>() const noexcept
{
	return attack_white;
}

}
