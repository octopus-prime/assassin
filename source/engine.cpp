/*
 * engine.cpp
 *
 *  Created on: 10.04.2016
 *      Author: mike_gresens
 */

//#define __AVX2__ 1

#include <io.hpp>
#include "board.hpp"
#include "color.hpp"
#include "move.hpp"
#include "node.hpp"
#include "attacker.hpp"
#include "generator.hpp"
#include "io.hpp"
#include <algorithm>
#include <sstream>
#include <cstring>

namespace chess {

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
		return node.color == white
			? generate<white_tag>(node, moves)
			: generate<black_tag>(node, moves);
	}

	template <typename color_tag>
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		moves = generator<king_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = generator<rook_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = generator<bishop_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = generator<knight_tag, color_tag>::template generate<moves_tag>(node, moves);
		moves = generator<pawn_tag, color_tag>::template generate<moves_tag>(node, moves);
		return moves;
	}

private:
	moves_t _moves;
	moves_t::iterator _end;
};

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

struct attack_tag;
struct defend_tag;
struct center_tag;

template <typename feature_tag>
struct evaluator;

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
evaluate(const node_t& node) noexcept // todo: what's broken??
{
	score_t score = node.score;
	score += evaluate<attack_tag>(node);
	score += evaluate<defend_tag>(node);
	score += evaluate<center_tag>(node);
	return score;
}

}

int main()
{
	using namespace chess;

//	node_t node {E1 | E4 | C2 | D4 | E6, E8 | F5 | C7, E6, D4, E4 | C7, C2 | F5, {}, e1, e8, black};
//	node.attack_white = attack_generator::generate<white_tag>(node);
//	node.attack_black = attack_generator::generate<black_tag>(node);
////	const node_t node {E1 | E4 | C2 | D4 | E6, E8 | G6 | C7, E6, D4, E4 | C7, C2 | G6, {}, e1, e8, white};

	node_t node;
	memset(&node, 0, sizeof(node_t));
	node.color = white;
	std::istringstream fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR");
	fen >> node;

	std::cout << node << std::endl;

	const move_generator<all_tag> moves(node);
	std::cout << moves.size() << std::endl;
	for (const auto move : moves)
		std::cout << move << std::endl;

	std::cout << std::bitset<64>(node.attack_white) << std::endl;
	std::cout << std::bitset<64>(node.attack_black) << std::endl;

	std::cout << evaluate(node) << std::endl;

	return 0;
}
