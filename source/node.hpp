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

struct white_tag {};
struct black_tag {};

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
	board_t white;
	board_t black;
	board_t rook;
	board_t bishop;
	board_t knight;
	board_t pawn;
	std::array<piece_t, 64> pieces;
	square_t king_w;
	square_t king_b;
	color_t color;
	board_t attack_w;
	board_t attack_b;

	template <typename piece_tag>
	constexpr board_t board_piece() const noexcept;

	template <typename color_tag>
	constexpr board_t board_occupy() const noexcept;

	constexpr board_t board_occupy() const noexcept
	{
		return white | black;
	}

	template <typename color_tag>
	constexpr square_t square_king() const noexcept;

	template <typename color_tag>
	constexpr board_t board_attack() const noexcept;
};

template <>
constexpr board_t
node_t::board_occupy<white_tag>() const noexcept
{
	return white;
}

template <>
constexpr board_t
node_t::board_occupy<black_tag>() const noexcept
{
	return black;
}

template <>
constexpr board_t
node_t::board_piece<king_tag>() const noexcept
{
	return board_of(king_w) | board_of(king_b);
}

template <>
constexpr board_t
node_t::board_piece<rook_tag>() const noexcept
{
	return rook;
}

template <>
constexpr board_t
node_t::board_piece<bishop_tag>() const noexcept
{
	return bishop;
}

template <>
constexpr board_t
node_t::board_piece<knight_tag>() const noexcept
{
	return knight;
}

template <>
constexpr board_t
node_t::board_piece<pawn_tag>() const noexcept
{
	return pawn;
}

template <>
constexpr square_t
node_t::square_king<white_tag>() const noexcept
{
	return king_w;
}

template <>
constexpr square_t
node_t::square_king<black_tag>() const noexcept
{
	return king_b;
}

template <>
constexpr board_t
node_t::board_attack<white_tag>() const noexcept
{
	return attack_w;
}

template <>
constexpr board_t
node_t::board_attack<black_tag>() const noexcept
{
	return attack_w;
}

}
