/*
 * move_generator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "node.hpp"
#include "move.hpp"
#include "attacker.hpp"
#include <algorithm>

namespace chess {
namespace detail {

template <typename moves_tag, typename color_tag>
struct masker;

template <typename color_tag>
struct masker<all_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return ~node.occupy<color_tag>();
	}
};

template <typename color_tag>
struct masker<passive_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return ~node.occupy();
	}
};

template <typename color_tag>
struct masker<active_tag, color_tag>
{
	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		return node.occupy<other_tag>();
	}
};

template <typename piece_tag, typename color_tag>
struct generator
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<piece_tag, color_tag>()))
			for (const auto to : bsf(attacker<piece_tag, color_tag>::attack(board_of(from), node.occupy()) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<knight_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<knight_tag, color_tag>()))
			for (const auto to : bsf(attacker<knight_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <typename color_tag>
struct generator<king_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		const square_t from = node.king<color_tag>();
		for (const auto to : bsf(attacker<king_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
			*moves++ = move_t {from, to};
		return castle<moves_tag>(node, moves);
	}

private:
	template <typename moves_tag>
	static moves_t::iterator
	castle(const node_t& node, moves_t::iterator moves) noexcept
	{
		return moves;
	}
};

template <>
template <>
inline moves_t::iterator
generator<king_tag, white_tag>::castle<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	if ((node.castle() & white_castle_king) && !(node.occupy() & (F1 | G1)) && !(node.attack<black_tag>() & (E1 | F1 | G1)))
		*moves++ = move_t {e1, g1};
	if ((node.castle() & white_castle_queen) && !(node.occupy() & (B1 | C1 | D1)) && !(node.attack<black_tag>() & (C1 | D1 | E1)))
		*moves++ = move_t {e1, c1};
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<king_tag, black_tag>::castle<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	if ((node.castle() & black_castle_king) && !(node.occupy() & (F8 | G8)) && !(node.attack<white_tag>() & (E8 | F8 | G8)))
		*moves++ = move_t {e8, g8};
	if ((node.castle() & black_castle_queen) && !(node.occupy() & (B8 | C8 | D8)) && !(node.attack<white_tag>() & (C8 | D8 | E8)))
		*moves++ = move_t {e8, c8};
	return moves;
}

template <typename color_tag>
struct generator<pawn_tag, color_tag>
{
	template <typename moves_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves) noexcept
	{
		for (const auto from : bsf(node.occupy<pawn_tag, color_tag>()))
			for (const auto to : bsf(attacker<pawn_tag, color_tag>::attack(board_of(from)) & masker<moves_tag, color_tag>::mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}
};

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, white_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	return moves;
}

template <>
template <>
inline moves_t::iterator
generator<pawn_tag, black_tag>::generate<all_tag>(const node_t& node, moves_t::iterator moves) noexcept
{
	return moves;
}

}


template <typename moves_tag>
class move_generator
{
public:
	move_generator(const node_t& node)
	:
		_end(generate(node, _moves.begin()))
	{
		std::stable_sort(_moves.begin(), _end, [](const move_t m1, const move_t m2){return false;});
	}

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

protected:
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		return node.color() == white
			? generate<white_tag>(node, moves)
			: generate<black_tag>(node, moves);
	}

	template <typename color_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		moves = detail::generator<king_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<rook_queen_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<bishop_queen_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<knight_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = detail::generator<pawn_tag, color_tag>::template generate<moves_tag>(node, moves);
		return moves;
	}

private:
	moves_t _moves;
	moves_t::iterator _end;
};

}
