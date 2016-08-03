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
#include <algorithm>

////namespace chess2 {
////
////namespace square {
////
////typedef std::uint_fast8_t type;
////
////constexpr type
////make(const type file, const type rank) noexcept
////{
////	return rank << 3 | file;
////}
////
////constexpr type
////rank(const type square) noexcept
////{
////	return square >> 3;
////}
////
////constexpr type
////file(const type square) noexcept
////{
////	return square & 7;
////}
////
////enum squares : type
////{
////	a1, h8, b2
////};
////
////}
////
////namespace board {
////
////typedef std::uint_fast64_t type;
////
////constexpr type
////make(const square::type square) noexcept
////{
////	return 1ULL << square;
////}
////
////enum squares : type
////{
////	A1, H8,
////	B2 = make(square::b2)
////};
////
////enum ranks : type
////{
////	R1
////};
////
////enum files : type
////{
////	Fa
////};
////
////}
////
////void foo()
////{
////	constexpr square::type s = square::make(0, 0);
////	constexpr board::type t = board::R1;
////}
////
////}
//
//template <typename color_tag>
//struct trait;
//
//template <>
//struct trait<white_tag>
//{
//	typedef black_tag invert;
//};
//
//template <>
//struct trait<black_tag>
//{
//	typedef white_tag invert;
//};
//}

namespace chess {

template <typename moves_tag, typename piece_tag, typename color_tag>
struct generator
{
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		for (const auto from : bsf(occupy(node)))
			for (const auto to : bsf(attack(node, from) & mask(node)))
				*moves++ = move_t {from, to};
		return moves;
	}

	static board_t
	generate(const node_t& node)
	{
		return attacker<piece_tag, color_tag>::attack(node, occupy(node));
	}

private:
	static board_t
	attack(const node_t& node, const square_t square)
	{
		return attacker<piece_tag, color_tag>::attack(node, board_of(square));
	}

	static board_t
	occupy(const node_t& node)
	{
		return node.board_piece<piece_tag>() & node.board_occupy<color_tag>();
	}

	static board_t
	mask(const node_t& node)
	{
		return masker<moves_tag, color_tag>::mask(node);
	}
};

template <typename moves_tag, typename color_tag>
struct generator<moves_tag, king_tag, color_tag>
{
	static moves_t::iterator
	generate(const node_t& node, moves_t::iterator moves)
	{
		const square_t from = node.square_king<color_tag>();
		for (const auto to : bsf(attack(node, from) & mask(node)))
			*moves++ = move_t {from, to};
		return moves;
	}

	static constexpr board_t
	generate(const node_t& node) noexcept
	{
		return attacker<king_tag, color_tag>::attack(node, occupy(node));
	}

private:
	static constexpr board_t
	attack(const node_t& node, const square_t square) noexcept
	{
		return attacker<king_tag, color_tag>::attack(node, board_of(square));
	}

	static constexpr board_t
	occupy(const node_t& node) noexcept
	{
		return board_of(node.square_king<color_tag>());
	}

	static constexpr board_t
	mask(const node_t& node) noexcept
	{
		return masker<moves_tag, color_tag>::mask(node);
	}
};

template <typename moves_tag>
struct attack_generator
{
	template <typename color_tag>
	static board_t
	generate(const node_t& node) noexcept
	{
		board_t board = 0;
		board |= generator<moves_tag, king_tag, color_tag>::generate(node);
		board |= generator<moves_tag, rook_tag, color_tag>::generate(node);
		board |= generator<moves_tag, bishop_tag, color_tag>::generate(node);
		board |= generator<moves_tag, knight_tag, color_tag>::generate(node);
		board |= generator<moves_tag, pawn_tag, color_tag>::generate(node);
		return board;
	}
};

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
		moves = generator<moves_tag, king_tag, color_tag>::generate(node, moves);
		moves = generator<moves_tag, rook_tag, color_tag>::generate(node, moves);
		moves = generator<moves_tag, bishop_tag, color_tag>::generate(node, moves);
		moves = generator<moves_tag, knight_tag, color_tag>::generate(node, moves);
		moves = generator<moves_tag, pawn_tag, color_tag>::generate(node, moves);
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

template <typename feature_tag, typename... color_tag>
struct evaluator;

template <typename... color_tag>
struct evaluator<attack_tag, color_tag...>
{
	static constexpr int weight = 5;

	static constexpr int
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag...>::other other_tag;
		return popcnt(node.board_attack<color_tag...>() & node.board_occupy<other_tag>());
	}
};

template <typename... color_tag>
struct evaluator<defend_tag, color_tag...>
{
	static constexpr int weight = 4;

	static constexpr int
	evaluate(const node_t& node) noexcept
	{
		return popcnt(node.board_attack<color_tag...>() & node.board_occupy<color_tag...>());
	}
};

template <typename... color_tag>
struct evaluator<center_tag, color_tag...>
{
	static constexpr int weight = 10;

	static constexpr int
	evaluate(const node_t& node) noexcept
	{
		constexpr board_t center = D4 | E4 | D5 | E5;
		return popcnt(node.board_occupy<color_tag...>() & center);
	}
};

template <typename feature_tag>
constexpr int
evaluate(const node_t& node) noexcept
{
	const int white = evaluator<feature_tag, white_tag>::evaluate(node);
	const int black = evaluator<feature_tag, black_tag>::evaluate(node);
	const int weight = evaluator<feature_tag>::weight;
	return (white - black) * weight;
}

constexpr int
evaluate(const node_t& node) noexcept
{
	int score = 0;
	score += evaluate<attack_tag>(node);
	score += evaluate<defend_tag>(node);
	score += evaluate<center_tag>(node);
	return score;
}

}

int main()
{
	using namespace chess;

	node_t node {E1 | E4 | C2 | D4 | E6, E8 | F5 | C7, E6, D4, E4 | C7, C2 | F5, {}, e1, e8, black};
	node.attack_w = attack_generator<active_tag>::generate<white_tag>(node);
	node.attack_b = attack_generator<active_tag>::generate<black_tag>(node);
//	const node_t node {E1 | E4 | C2 | D4 | E6, E8 | G6 | C7, E6, D4, E4 | C7, C2 | G6, {}, e1, e8, white};

	const move_generator<all_tag> moves(node);
	std::cout << moves.size() << std::endl;
	for (const auto move : moves)
		std::cout << move << std::endl;

	const board_t board = attack_generator<active_tag>::generate<black_tag>(node);
	std::cout << std::bitset<64>(board) << std::endl;

	std::cout << evaluate(node) << std::endl;
	std::cout << sizeof(~0ULL) << std::endl;

	return 0;
}
