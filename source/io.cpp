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
operator<<(ostream& stream, const chess::square square)
{
	stream << char('a' + chess::file_of(square));
	stream << char('1' + chess::rank_of(square));
	return stream;
}

ostream&
operator<<(ostream& stream, const move_t& move)
{
	stream << square(move.from);
	stream << square(move.to);
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
	stream << "EnPassant = ";
	if (node.en_passant())
		stream << square(node.en_passant());
	stream << endl;
	return stream;
}

ostream&
operator<<(ostream& stream, const std::pair<const chess::node_t&, const chess::move_t&>& pair)
{
	static constexpr std::array<const char* const, 13> piece
	{{
		"",
		"K", "Q", "R", "B", "N", "P",
		"k", "q", "r", "b", "n", "p"
	}};
	static constexpr std::array<const char* const, 5> promoted
	{{
		"", "N", "B", "R", "Q"
	}};
	static constexpr std::array<const char* const, 2> check
	{{
		"", "+"
	}};
	stream << piece[pair.first[pair.second.from]];
	stream << square(pair.second.from);
	stream << (pair.first[pair.second.to] != no_piece || (pair.first.en_passant() && pair.second.to == pair.first.en_passant()) ? 'x' : '-');
	stream << square(pair.second.to);
	stream << promoted[pair.second.promotion];
	stream << check[pair.second.index];
	return stream;
}

}
