/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

//#define __AVX2__ 1

#include "board.hpp"
#include "color.hpp"
#include "move.hpp"
#include "node.hpp"
#include "attacker.hpp"
#include "move_generator.hpp"
#include "io.hpp"
#include <algorithm>
#include <sstream>
#include <cstring>

namespace chess {

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

struct mobility_tag;
struct attack_tag;
struct defend_tag;
struct center_tag;

template <typename feature_tag>
struct evaluator;

template <>
struct evaluator<mobility_tag>
{
	static constexpr score_t weight = 2;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		return popcnt(node.attack<color_tag>());
	}
};

template <>
struct evaluator<attack_tag>
{
	static constexpr score_t weight = 5;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		return popcnt(node.attack<color_tag>() & node.occupy<other_tag>());
	}
};

template <>
struct evaluator<defend_tag>
{
	static constexpr score_t weight = 4;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		return popcnt(node.attack<color_tag>() & node.occupy<color_tag>());
	}
};

template <>
struct evaluator<center_tag>
{
	static constexpr score_t weight = 10;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		constexpr board_t center = D4 | E4 | D5 | E5;
		return popcnt(node.occupy<color_tag>() & center);
	}
};

template <typename feature_tag>
constexpr score_t
evaluate(const node_t& node) noexcept
{
	const score_t white = evaluator<feature_tag>::template evaluate<white_tag>(node);
	const score_t black = evaluator<feature_tag>::template evaluate<black_tag>(node);
	const score_t weight = evaluator<feature_tag>::weight;
	return (white - black) * weight;
}

constexpr score_t
evaluate(const node_t& node) noexcept
{
	score_t score = node.score();
	score += evaluate<mobility_tag>(node);
	score += evaluate<attack_tag>(node);
	score += evaluate<defend_tag>(node);
	score += evaluate<center_tag>(node);
	return score;
}

}

int main()
{
	using namespace chess;

	std::cout << sizeof(node_t) << std::endl;

	const node_t node("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR w");
	std::cout << node << std::endl;

	const move_generator<all_tag> moves(node);
	std::cout << moves.size() << std::endl;
	for (const auto move : moves)
		std::cout << move << std::endl;

	std::cout << std::bitset<64>(node.attack<white_tag>()) << std::endl;
	std::cout << std::bitset<64>(node.attack<black_tag>()) << std::endl;

	std::cout << evaluate(node) << std::endl;

	return 0;
}
