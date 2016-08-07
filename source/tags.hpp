/*
 * tags.hpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#pragma once

namespace chess {

struct white_tag;
struct black_tag;

struct king_tag;
struct queen_tag;
struct rook_tag;
struct rook_queen_tag;
struct bishop_tag;
struct bishop_queen_tag;
struct knight_tag;
struct pawn_tag;
struct sliding_tag;

struct all_tag;
struct active_tag;
struct passive_tag;

template <typename color_tag>
struct color_traits;

template <>
struct color_traits<white_tag>
{
	typedef black_tag other;
};

template <>
struct color_traits<black_tag>
{
	typedef white_tag other;
};

}
