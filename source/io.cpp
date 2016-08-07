/*
 * io.cpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#include "io.hpp"
#include <array>

namespace std {

using namespace chess;

ostream&
operator<<(ostream& stream, const bitset<64>& board)
{
	static constexpr array<char, 2> pieces = {' ', '*'};
	stream << endl << " abcdefgh" << endl;
	for (square_t rank = 8; rank > 0; --rank)
	{
		stream << short(rank);
		for (square_t file = 0; file < 8; ++file)
		{
			const square_t square = square_of(file, rank - 1);
			const bool piece = board[square];
			stream << pieces[piece];
		}
		stream << short(rank) << endl;
	}
	stream << " abcdefgh" << endl;
	return stream;
}

ostream&
operator<<(ostream& stream, const move_t& move)
{
	stream << char('a' + file_of(move.from));
	stream << char('1' + rank_of(move.from));
	stream << char('a' + file_of(move.to));
	stream << char('1' + rank_of(move.to));
	return stream;
}

ostream&
operator<<(ostream& stream, const node_t& node)
{
	static constexpr std::array<char, 13> pieces
	{{
		' ',
		'K', 'Q', 'R', 'B', 'N', 'P',
		'k', 'q', 'r', 'b', 'n', 'p'
	}};
	stream << endl << " abcdefgh" << endl;
	for (square_rank_t rank = 8; rank > 0; --rank)
	{
		stream << short(rank);
		for (square_file_t file = 0; file < 8; ++file)
		{
			const square_t square = square_of(file, rank - 1);
			const piece_t piece = node[square];
			stream << pieces[piece];
		}
		stream << short(rank) << endl;
	}
	stream << " abcdefgh" << endl;
	stream << "Color = " << node.color() << endl;
//	stream << "EnPassant = ";
//	stream << char('a' + file_of(node.square_en_passant()));
//	stream << char('1' + rank_of(node.square_en_passant()));
//	stream << endl;
	return stream;
}

}
