/*
 * exchanger.cpp
 *
 *  Created on: 18.08.2016
 *      Author: mike
 */

#include "exchanger.hpp"
#include "attacker.hpp"



#include "io.hpp"

namespace chess {
namespace detail {

constexpr std::array<score_t, 5> promotion {0, 1_N - 1_P, 1_B - 1_P, 1_R - 1_P, 1_Q - 1_P};

struct exchanger_t
{
	typedef std::array<score_t, 32> scores_t;
	typedef std::array<square_t, 16> squares_t;

public:
	constexpr exchanger_t(const node_t& node) noexcept
	:
		_node(node),
		_white(node.occupy<white_tag>()),
		_black(node.occupy<black_tag>())
	{
//		std::cout << std::bitset<64>(_white) << std::endl;
//		std::cout << std::bitset<64>(_black) << std::endl;
	}

	score_t operator()(const move_t move) noexcept;

protected:
	template <typename color_tag>
	squares_t::iterator fill(const board_t board, const board_t color, squares_t::iterator iterator) const noexcept;

	square_t find(const board_t board, const board_t color) const noexcept;

private:
	const node_t& _node;
	board_t _white;
	board_t _black;
};

template <typename color_tag>
exchanger_t::squares_t::iterator
exchanger_t::fill(const board_t board, const board_t color, squares_t::iterator iterator) const noexcept
{
	typedef typename color_traits<color_tag>::other other_tag;

	if (_node.occupy<knight_tag>() & color)
	{
		const board_t knights = attacker<knight_tag, color_tag>::attack(board) & _node.occupy<knight_tag>() & color;
		for (const square_t knight : bsf(knights))
			*iterator++ = knight;
	}

	if (_node.occupy<pawn_tag>() & color)
	{
		const board_t pawns = attacker<pawn_tag, other_tag>::attack(board) & _node.occupy<pawn_tag>() & color;
		for (const square_t pawn : bsf(pawns))
			*iterator++ = pawn;
	}

	return iterator;
}

square_t
exchanger_t::find(const board_t board, const board_t color) const noexcept
{
	if (color & (_node.occupy<rook_queen_tag>() | _node.occupy<bishop_queen_tag>()))
	{
		const board_t sliders = attacker<sliding_tag, white_tag>::attack(board, board, _white | _black) & color;
		const board_t bishops = sliders & _node.occupy<bishop_tag>();
		if (bishops)
			return __bsfq(bishops);
		const board_t rooks = sliders & _node.occupy<rook_tag>();
		if (rooks)
			return __bsfq(rooks);
		const board_t queens = sliders & _node.occupy<queen_tag>();
		if (queens)
			return __bsfq(queens);
	}

	if (color & _node.occupy<king_tag>())
	{
		const board_t kings = attacker<king_tag, white_tag>::attack(board) & _node.occupy<king_tag>() & color;
		if (kings)
			return __bsfq(kings);
	}

	// todo: kings
	return 64; // not found
}

score_t
exchanger_t::operator()(const move_t move) noexcept
{
	scores_t scores;
	squares_t squares_w;
	squares_t squares_b;

	// --------------

	color_t color = _node.color();
	if (color == white)
		_white ^= board_of(move.from);
	else
		_black ^= board_of(move.from);
	color *= -1;

	scores_t::iterator scores_end = scores.begin();
	*scores_end++ = score_of[_node[move.to]] + promotion[move.promotion];

	// -------------------------------

	squares_t::iterator squares_w_end = fill<white_tag>(board_of(move.to), _white, squares_w.begin());
	squares_t::iterator squares_b_end = fill<black_tag>(board_of(move.to), _black, squares_b.begin());

//	std::cout << std::bitset<64>(_white) << std::endl;
//	std::cout << std::bitset<64>(_black) << std::endl;
	// ---------------------------------

	score_t score = score_of[_node[move.from]] + promotion[move.promotion];
	while (scores_end < scores.end())
	{
		square_t square;
		if (color == white)
		{
			square = squares_w_end > squares_w.begin()
				? *--squares_w_end
				: find(board_of(move.to), _white);
			if (square == 64)
				break;
			_white ^= board_of(square);
//			std::cout << std::bitset<64>(_white) << std::endl;
		}
		else
		{
			square = squares_b_end > squares_b.begin()
				? *--squares_b_end
				: find(board_of(move.to), _black);
			if (square == 64)
				break;
			_black ^= board_of(square);
//			std::cout << std::bitset<64>(_black) << std::endl;
		}
		*scores_end = score - *(scores_end - 1);
		score = score_of[_node[square]];
		++scores_end;
		color *= -1;
//		std::cout << int(square) << std::endl;
//		std::cout << std::endl;
	}

	while (scores_end > scores.begin() + 1)
	{
		--scores_end;
		if(*(scores_end - 1) > -*scores_end)
			*(scores_end - 1) = -*scores_end;
	}

	return scores.front();
}

}

score_t
exchanger::evaluate(const node_t& node, const move_t move) noexcept
{
	const score_t score = score_of[node[move.to]] + detail::promotion[move.promotion];
	if (score == 0)
		return 0;
	return detail::exchanger_t(node)(move);
}

}
