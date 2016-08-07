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
#include "score.hpp"
#include "tags.hpp"
#include "move.hpp"
#include <array>
#include <string>

namespace chess {

class node_t
{
	struct creator;

public:
	static const std::string INITIAL_POSITION;

	constexpr node_t();
	explicit node_t(const std::string& fen);
	explicit node_t(const node_t& node, const move_t& move);

	template <typename piece_or_color_tag>
	constexpr board_t occupy() const noexcept;

	template <typename piece_or_color_tag1, typename piece_or_color_tag2>
	constexpr board_t occupy() const noexcept;

	constexpr board_t occupy() const noexcept;

	template <typename color_tag>
	constexpr square_t king() const noexcept;

	template <typename color_tag>
	constexpr board_t attack() const noexcept;

	constexpr piece_t operator[](const std::size_t index) const noexcept;

	constexpr color_t color() const noexcept;

	constexpr score_t score() const noexcept;

	constexpr const node_t* const parent() const noexcept;

	constexpr square_t en_passant() const noexcept;

	constexpr castle_t castle() const noexcept;

	constexpr std::uint8_t half_moves() const noexcept;
	constexpr std::uint8_t full_moves() const noexcept;

private:
	const node_t* const _parent;
	board_t _occupy_white;
	board_t _occupy_black;
	board_t _occupy_rook_queen;
	board_t _occupy_bishop_queen;
	board_t _occupy_knight;
	board_t _occupy_pawn;
	board_t _attack_white;
	board_t _attack_black;
	hash_t _hash;
	std::array<piece_t, 64> _pieces;
	score_t _score;
	square_t _king_white;
	square_t _king_black;
	square_t _en_passant;
	color_t _color;
	castle_t _castle;
	std::uint8_t _half_moves;
	std::uint8_t _full_moves;
};

constexpr
node_t::node_t()
:
	_parent {},
	_occupy_white {},
	_occupy_black {},
	_occupy_rook_queen {},
	_occupy_bishop_queen {},
	_occupy_knight {},
	_occupy_pawn {},
	_attack_white {},
	_attack_black {},
	_hash {},
	_pieces {},
	_score {},
	_king_white {},
	_king_black {},
	_en_passant {},
	_color {},
	_castle {},
	_half_moves {},
	_full_moves {}
{
}

constexpr const node_t* const
node_t::parent() const noexcept
{
	return _parent;
}

constexpr piece_t
node_t::operator[](const std::size_t index) const noexcept
{
	return _pieces[index];
}

constexpr color_t
node_t::color() const noexcept
{
	return _color;
}

constexpr score_t
node_t::score() const noexcept
{
	return _score;
}

constexpr square_t
node_t::en_passant() const noexcept
{
	return _en_passant;
}

constexpr castle_t
node_t::castle() const noexcept
{
	return _castle;
}

constexpr std::uint8_t
node_t::half_moves() const noexcept
{
	return _half_moves;
}

constexpr std::uint8_t
node_t::full_moves() const noexcept
{
	return _full_moves;
}

template <>
constexpr square_t
node_t::king<white_tag>() const noexcept
{
	return _king_white;
}

template <>
constexpr square_t
node_t::king<black_tag>() const noexcept
{
	return _king_black;
}

template <>
constexpr board_t
node_t::attack<white_tag>() const noexcept
{
	return _attack_white;
}

template <>
constexpr board_t
node_t::attack<black_tag>() const noexcept
{
	return _attack_black;
}

constexpr board_t
node_t::occupy() const noexcept
{
	return _occupy_white | _occupy_black;
}

template <>
constexpr board_t
node_t::occupy<white_tag>() const noexcept
{
	return _occupy_white;
}

template <>
constexpr board_t
node_t::occupy<black_tag>() const noexcept
{
	return _occupy_black;
}

template <>
constexpr board_t
node_t::occupy<king_tag>() const noexcept
{
	return board_of(_king_white) | board_of(_king_black);
}

template <>
constexpr board_t
node_t::occupy<rook_queen_tag>() const noexcept
{
	return _occupy_rook_queen;
}

template <>
constexpr board_t
node_t::occupy<queen_tag>() const noexcept
{
	return _occupy_rook_queen & _occupy_bishop_queen;
}

template <>
constexpr board_t
node_t::occupy<rook_tag>() const noexcept
{
	return _occupy_rook_queen & ~_occupy_bishop_queen;
}

template <>
constexpr board_t
node_t::occupy<bishop_queen_tag>() const noexcept
{
	return _occupy_bishop_queen;
}

template <>
constexpr board_t
node_t::occupy<bishop_tag>() const noexcept
{
	return _occupy_bishop_queen & ~_occupy_rook_queen;
}

template <>
constexpr board_t
node_t::occupy<knight_tag>() const noexcept
{
	return _occupy_knight;
}

template <>
constexpr board_t
node_t::occupy<pawn_tag>() const noexcept
{
	return _occupy_pawn;
}

template <typename piece_or_color_tag1, typename piece_or_color_tag2>
constexpr board_t
node_t::occupy() const noexcept
{
	return occupy<piece_or_color_tag1>() & occupy<piece_or_color_tag2>();
}

template <>
constexpr board_t
node_t::occupy<white_tag, king_tag>() const noexcept
{
	return board_of(king<white_tag>());
}

template <>
constexpr board_t
node_t::occupy<black_tag, king_tag>() const noexcept
{
	return board_of(king<black_tag>());
}

template <>
constexpr board_t
node_t::occupy<king_tag, white_tag>() const noexcept
{
	return board_of(king<white_tag>());
}

template <>
constexpr board_t
node_t::occupy<king_tag, black_tag>() const noexcept
{
	return board_of(king<black_tag>());
}

}
