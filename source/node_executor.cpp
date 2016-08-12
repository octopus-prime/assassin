/*
 * node_executor.cpp
 *
 *  Created on: 11.08.2016
 *      Author: mike
 */

#include "node.hpp"
#include "attack_generator.hpp"
//#include "io.hpp"
#include <boost/format.hpp>

namespace chess {
/*
class illegal_exception
:
	public std::exception
{
private:
	std::string _what;
	node_t _node;
	move_t _move;

public:
	illegal_exception(const std::string& what, const node_t& node, const move_t move) noexcept
	:
		std::exception(),
		_what(what),
		_node(node),
		_move(move)
	{
	}

	virtual ~illegal_exception() noexcept
	{
	}

	virtual const char* what() const noexcept
	{
		boost::format format("What = %s\nNode = %s\nMove = %s\n");
		format % _what;
		format % _node;
		format % std::make_pair(std::ref(_node), std::ref(_move));
		return format.str().data();
	}
};
*/
struct node_t::executor
{
	static void
	execute(const node_t& node, const move_t& move, node_t& successor);

private:
	static void
	shift(node_t& successor, const square_t from, const square_t to) noexcept
	{
		const piece_t piece = successor[from];
		successor._hash ^= piece_square_hash[piece][from];
		successor._hash ^= piece_square_hash[piece][to];
		successor._pieces[to] = piece;
		successor._pieces[from] = no_piece;
	}

	static square_t
	flip(node_t& successor, const square_t enPassant) noexcept
	{
		const square_t result = successor._en_passant;
		successor._hash ^= en_passant_hash[result];
		successor._en_passant = enPassant;
		successor._hash ^= en_passant_hash[enPassant];
		successor._hash ^= color_hash;
		successor._color *= -1;
		return result;
	}

	template <piece_t piece>
	static square_t
	execute_move(node_t& successor, const square_t from, const square_t to) noexcept;

	template <piece_t piece>
	static void
	execute_capture(node_t& successor, const square_t to) noexcept;

	template <piece_t piece>
	static void
	execute_promote(node_t& successor, const square_t to) noexcept;
};

template <>
square_t
node_t::executor::execute_move<K>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._king_white 			= squareTo;
	if (successor._castle & white_castle)
	{
		switch (squareFrom - squareTo)
		{
		case -2:
			successor._occupy_white			^= H1 | F1;
			successor._occupy_rook_queen	^= H1 | F1;
			shift(successor, h1, f1);
			break;
		case +2:
			successor._occupy_white			^= A1 | D1;
			successor._occupy_rook_queen	^= A1 | D1;
			shift(successor, a1, d1);
			break;
		default:
			break;
		}
		successor._hash		^= castle_hash[successor._castle];
		successor._castle	&= ~white_castle;
		successor._hash		^= castle_hash[successor._castle];
	}
	return 0;
}

template <>
square_t
node_t::executor::execute_move<k>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._king_black 			= squareTo;
	if (successor._castle & black_castle)
	{
		switch (squareFrom - squareTo)
		{
		case -2:
			successor._occupy_black			^= H8 | F8;
			successor._occupy_rook_queen	^= H8 | F8;
			shift(successor, h8, f8);
			break;
		case +2:
			successor._occupy_black			^= A8 | D8;
			successor._occupy_rook_queen	^= A8 | D8;
			shift(successor, a8, d8);
			break;
		default:
			break;
		}
		successor._hash		^= castle_hash[successor._castle];
		successor._castle	&= ~black_castle;
		successor._hash		^= castle_hash[successor._castle];
	}
	return 0;
}

template <>
square_t
node_t::executor::execute_move<Q>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._occupy_rook_queen	^= boardFrom;
	successor._occupy_rook_queen	|= boardTo;
	successor._occupy_bishop_queen	^= boardFrom;
	successor._occupy_bishop_queen	|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<q>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._occupy_rook_queen	^= boardFrom;
	successor._occupy_rook_queen	|= boardTo;
	successor._occupy_bishop_queen	^= boardFrom;
	successor._occupy_bishop_queen	|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<R>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._occupy_rook_queen	^= boardFrom;
	successor._occupy_rook_queen	|= boardTo;
	if (successor._castle & white_castle)
	{
		successor._hash ^= castle_hash[successor._castle];
		switch (squareFrom)
		{
		case h1:
			successor._castle &= ~white_castle_king;
			break;
		case a1:
			successor._castle &= ~white_castle_queen;
			break;
		default:
			break;
		}
		successor._hash ^= castle_hash[successor._castle];
	}
	return 0;
}

template <>
square_t
node_t::executor::execute_move<r>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._occupy_rook_queen	^= boardFrom;
	successor._occupy_rook_queen	|= boardTo;
	if (successor._castle & black_castle)
	{
		successor._hash ^= castle_hash[successor._castle];
		switch (squareFrom)
		{
		case h8:
			successor._castle &= ~black_castle_king;
			break;
		case a8:
			successor._castle &= ~black_castle_queen;
			break;
		default:
			break;
		}
		successor._hash ^= castle_hash[successor._castle];
	}
	return 0;
}

template <>
square_t
node_t::executor::execute_move<B>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._occupy_bishop_queen	^= boardFrom;
	successor._occupy_bishop_queen	|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<b>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._occupy_bishop_queen	^= boardFrom;
	successor._occupy_bishop_queen	|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<N>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._occupy_knight		^= boardFrom;
	successor._occupy_knight		|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<n>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._occupy_knight		^= boardFrom;
	successor._occupy_knight		|= boardTo;
	return 0;
}

template <>
square_t
node_t::executor::execute_move<P>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_white			^= mask;
	successor._occupy_pawn			^= boardFrom;
	successor._occupy_pawn			|= boardTo;
	successor._half_moves			= 0;
	return (squareTo - squareFrom == 16) ? squareFrom + 8 : 0;
}

template <>
square_t
node_t::executor::execute_move<p>(node_t& successor, const square_t squareFrom, const square_t squareTo) noexcept
{
	const board_t boardFrom			= board_of(squareFrom);
	const board_t boardTo			= board_of(squareTo);
	const board_t mask				= boardFrom | boardTo;
	successor._occupy_black			^= mask;
	successor._occupy_pawn			^= boardFrom;
	successor._occupy_pawn			|= boardTo;
	successor._half_moves			= 0;
	return (squareFrom - squareTo == 16) ? squareFrom - 8 : 0;
}

template <>
void
node_t::executor::execute_capture<Q>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_white			^= board_of(to);
	successor._occupy_rook_queen	^= board_of(to);
	successor._occupy_bishop_queen	^= board_of(to);
	successor._score				-= score_of[Q];
}

template <>
void
node_t::executor::execute_capture<q>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_black			^= board_of(to);
	successor._occupy_rook_queen	^= board_of(to);
	successor._occupy_bishop_queen	^= board_of(to);
	successor._score				+= score_of[q];
}

template <>
void
node_t::executor::execute_capture<R>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_white			^= board_of(to);
	successor._occupy_rook_queen	^= board_of(to);
	successor._score				-= score_of[R];
	if (successor._castle & white_castle)
	{
		successor._hash ^= castle_hash[successor._castle];
		switch (to)
		{
		case h1:
			successor._castle &= ~white_castle_king;
			break;
		case a1:
			successor._castle &= ~white_castle_queen;
			break;
		default:
			break;
		}
		successor._hash ^= castle_hash[successor._castle];
	}
}

template <>
void
node_t::executor::execute_capture<r>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_black			^= board_of(to);
	successor._occupy_rook_queen	^= board_of(to);
	successor._score				+= score_of[r];
	if (successor._castle & black_castle)
	{
		successor._hash ^= castle_hash[successor._castle];
		switch (to)
		{
		case h8:
			successor._castle &= ~black_castle_king;
			break;
		case a8:
			successor._castle &= ~black_castle_queen;
			break;
		default:
			break;
		}
		successor._hash ^= castle_hash[successor._castle];
	}
}

template <>
void
node_t::executor::execute_capture<B>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_white			^= board_of(to);
	successor._occupy_bishop_queen	^= board_of(to);
	successor._score				-= score_of[B];
}

template <>
void
node_t::executor::execute_capture<b>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_black			^= board_of(to);
	successor._occupy_bishop_queen	^= board_of(to);
	successor._score				+= score_of[b];
}

template <>
void
node_t::executor::execute_capture<N>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_white			^= board_of(to);
	successor._occupy_knight		^= board_of(to);
	successor._score				-= score_of[N];
}

template <>
void
node_t::executor::execute_capture<n>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_black			^= board_of(to);
	successor._occupy_knight		^= board_of(to);
	successor._score				+= score_of[n];
}

template <>
void
node_t::executor::execute_capture<P>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_white			^= board_of(to);
	successor._occupy_pawn			^= board_of(to);
	successor._score				-= score_of[P];
}

template <>
void
node_t::executor::execute_capture<p>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_black			^= board_of(to);
	successor._occupy_pawn			^= board_of(to);
	successor._score				+= score_of[p];
}

template <>
void
node_t::executor::execute_promote<Q>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_rook_queen	|= board_of(to);
	successor._occupy_bishop_queen	|= board_of(to);
	successor._score				+= score_of[Q] - score_of[P];
}

template <>
void
node_t::executor::execute_promote<q>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_rook_queen	|= board_of(to);
	successor._occupy_bishop_queen	|= board_of(to);
	successor._score				-= score_of[q] - score_of[p];
}

template <>
void
node_t::executor::execute_promote<R>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_rook_queen	|= board_of(to);
	successor._score				+= score_of[R] - score_of[P];
}

template <>
void
node_t::executor::execute_promote<r>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_rook_queen	|= board_of(to);
	successor._score				-= score_of[r] - score_of[p];
}

template <>
void
node_t::executor::execute_promote<B>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_bishop_queen	|= board_of(to);
	successor._score				+= score_of[B] - score_of[P];
}

template <>
void
node_t::executor::execute_promote<b>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_bishop_queen	|= board_of(to);
	successor._score				-= score_of[b] - score_of[p];
}

template <>
void
node_t::executor::execute_promote<N>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_knight		|= board_of(to);
	successor._score				+= score_of[N] - score_of[P];
}

template <>
void
node_t::executor::execute_promote<n>(node_t& successor, const square_t to) noexcept
{
	successor._occupy_knight		|= board_of(to);
	successor._score				-= score_of[n] - score_of[p];
}

void
node_t::executor::execute(const node_t& node, const move_t& move, node_t& successor)
{
	successor._parent = &node;
	successor._half_moves ++;
	successor._full_moves += (node.color() == black);

	const std::uint_fast8_t	index_promoted	= move.promotion;
	const piece_t			piece_moved		= node[move.from];
		  piece_t			piece_captured	= node[move.to];

	if (successor.en_passant() && (successor.en_passant() == move.to))
	{
		square_t index = move.to;
		switch (piece_moved)
		{
		case P:
			index -= 8;
			successor._occupy_black ^= board_of(index);
			successor._occupy_black |= board_of(move.to);
			piece_captured = p;
			successor._occupy_pawn ^= board_of(index);
			successor._occupy_pawn |= board_of(move.to);
			shift(successor, index, move.to);
			break;
		case p:
			index += 8;
			successor._occupy_white ^= board_of(index);
			successor._occupy_white |= board_of(move.to);
			piece_captured = P;
			successor._occupy_pawn ^= board_of(index);
			successor._occupy_pawn |= board_of(move.to);
			shift(successor, index, move.to);
			break;
		default:
			break;
		}
	}

	if (piece_captured)
	{
		successor._hash ^= piece_square_hash[piece_captured][move.to];

		switch (piece_captured)
		{
		case Q:
			execute_capture<Q>(successor, move.to);
			break;
		case q:
			execute_capture<q>(successor, move.to);
			break;
		case R:
			execute_capture<R>(successor, move.to);
			break;
		case r:
			execute_capture<r>(successor, move.to);
			break;
		case B:
			execute_capture<B>(successor, move.to);
			break;
		case b:
			execute_capture<b>(successor, move.to);
			break;
		case N:
			execute_capture<N>(successor, move.to);
			break;
		case n:
			execute_capture<n>(successor, move.to);
			break;
		case P:
			execute_capture<P>(successor, move.to);
			break;
		case p:
			execute_capture<p>(successor, move.to);
			break;
		default:
			throw std::runtime_error("Illegal capture");//, node, move);
			break;
		}

		successor._half_moves = 0;
	}

	square_t en_passant = 0;
	switch (piece_moved)
	{
	case K:
		execute_move<K>(successor, move.from, move.to);
		break;
	case k:
		execute_move<k>(successor, move.from, move.to);
		break;
	case Q:
		execute_move<Q>(successor, move.from, move.to);
		break;
	case q:
		execute_move<q>(successor, move.from, move.to);
		break;
	case R:
		execute_move<R>(successor, move.from, move.to);
		break;
	case r:
		execute_move<r>(successor, move.from, move.to);
		break;
	case B:
		execute_move<B>(successor, move.from, move.to);
		break;
	case b:
		execute_move<b>(successor, move.from, move.to);
		break;
	case N:
		execute_move<N>(successor, move.from, move.to);
		break;
	case n:
		execute_move<n>(successor, move.from, move.to);
		break;
	case P:
		en_passant = execute_move<P>(successor, move.from, move.to);
		break;
	case p:
		en_passant = execute_move<p>(successor, move.from, move.to);
		break;
	default:
		throw std::runtime_error("Illegal move");//, node, move);
		break;
	}

	successor._hash ^= piece_square_hash[piece_moved][move.from];

	if (index_promoted)
	{
		const piece_t piece_promoted	= piece_moved - index_promoted;
		successor._pieces[move.to]		= piece_promoted;
		successor._occupy_pawn			^= board_of(move.to);

		switch (piece_promoted)
		{
		case Q:
			execute_promote<Q>(successor, move.to);
			break;
		case q:
			execute_promote<q>(successor, move.to);
			break;
		case R:
			execute_promote<R>(successor, move.to);
			break;
		case r:
			execute_promote<r>(successor, move.to);
			break;
		case B:
			execute_promote<B>(successor, move.to);
			break;
		case b:
			execute_promote<b>(successor, move.to);
			break;
		case N:
			execute_promote<N>(successor, move.to);
			break;
		case n:
			execute_promote<n>(successor, move.to);
			break;
		default:
			throw std::runtime_error("Illegal promote");//, node, move);
			break;
		}
	}
	else
		successor._pieces[move.to] = piece_moved;

	successor._pieces[move.from] = no_piece;
	successor._hash ^= piece_square_hash[successor[move.to]][move.to];
	flip(successor, en_passant);

	successor._attack_white = attack_generator::generate<white_tag>(successor);
	successor._attack_black = attack_generator::generate<black_tag>(successor);
}

node_t::node_t(const node_t& node, const move_t& move)
:
	node_t(node)
{
	executor::execute(node, move, *this);
}

}
