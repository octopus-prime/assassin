/*
 * analyzer.cpp
 *
 *  Created on: 27.08.2016
 *      Author: mike
 */

#include "analyzer.hpp"

namespace chess {
namespace detail {

/*
    opposed    = theirPawns & forward_bb(Us, s);
    stoppers   = theirPawns & passed_pawn_mask(Us, s);
    lever      = theirPawns & pawnAttacksBB[s];
    doubled    = ourPawns   & (s + Up);
    neighbours = ourPawns   & adjacent_files_bb(f);
    phalanx    = neighbours & rank_bb(s);
    supported  = neighbours & rank_bb(s - Up);
    connected  = supported | phalanx;


            // Score this pawn
    if (!neighbours)
        score -= Isolated[opposed];

    else if (backward)
        score -= Backward[opposed];

    else if (!supported)
        score -= Unsupported[more_than_one(neighbours & pawnAttacksBB[s])];

    if (connected)
        score += Connected[opposed][!!phalanx][more_than_one(supported)][relative_rank(Us, s)];

    if (doubled)
        score -= Doubled;

    if (lever)
        score += Lever[relative_rank(Us, s)];
}

 */

template <>
pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::make_color_0() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_doubled = popcnt(file) > 1;
			const bool is_blocked = rank < __bsrq(file);
			entries[rank][file] = (is_doubled ? doubled : 0) | (is_blocked ? blocked : 0);
		}
	return std::move(entries);
}

template <>
pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::make_color_0() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_doubled = popcnt(file) > 1;
			const bool is_blocked = rank > __bsfq(file);
			entries[rank][file] = (is_doubled ? doubled : 0) | (is_blocked ? blocked : 0);
		}
	return std::move(entries);
}

template <>
pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::make_other_0() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_blocked = file ? rank < __bsrq(file) : false;
			entries[rank][file] = is_blocked ? blocked : 0;
		}
	return std::move(entries);
}

template <>
pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::make_other_0() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_blocked = file ? rank > __bsfq(file) : false;
			entries[rank][file] = is_blocked ? blocked : 0;
		}
	return std::move(entries);
}

template <typename color_tag>
typename pawn_analyser_t<color_tag>::entries_t
pawn_analyser_t<color_tag>::make_color_1() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
			entries[rank][file] = popcnt(file) > 0 ? neighbour : 0;
	return std::move(entries);
}

template <>
pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::make_other_1() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_blocked = file ? rank < __bsrq(file) : false;
			entries[rank][file] = is_blocked ? blocked : 0;
		}
	return std::move(entries);
}

template <>
pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::make_other_1() noexcept
{
	entries_t entries;
	for (auto rank = 0; rank < 6; ++rank)
		for (auto file = 0; file < 64; ++file)
		{
			const bool is_blocked = file ? rank > __bsfq(file) : false;
			entries[rank][file] = is_blocked ? blocked : 0;
		}
	return std::move(entries);
}

template <>
const pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::properties_color_0 = pawn_analyser_t<white_tag>::make_color_0();

template <>
const pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::properties_other_0 = pawn_analyser_t<white_tag>::make_other_0();

template <>
const pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::properties_color_1 = pawn_analyser_t<white_tag>::make_color_1();

template <>
const pawn_analyser_t<white_tag>::entries_t
pawn_analyser_t<white_tag>::properties_other_1 = pawn_analyser_t<white_tag>::make_other_1();

template <>
const pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::properties_color_0 = pawn_analyser_t<black_tag>::make_color_0();

template <>
const pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::properties_other_0 = pawn_analyser_t<black_tag>::make_other_0();

template <>
const pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::properties_color_1 = pawn_analyser_t<black_tag>::make_color_1();

template <>
const pawn_analyser_t<black_tag>::entries_t
pawn_analyser_t<black_tag>::properties_other_1 = pawn_analyser_t<black_tag>::make_other_1();

}
}
