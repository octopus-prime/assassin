/*
 * io.cpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#include "io.hpp"
#include <array>

namespace chess {

}

namespace std {

using namespace chess;

ostream&
operator<<(ostream& stream, const bitset<64>& board)
{
	constexpr array<char, 2> pieces = {' ', '*'};
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

}
