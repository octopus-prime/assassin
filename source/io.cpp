/*
 * io.cpp
 *
 *  Created on: 06.08.2016
 *      Author: mike
 */

#include "io.hpp"
#include "hash.hpp"
#include "generator.hpp"
#include <array>
/*
#include <unordered_map>
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/include/support_istream_iterator.hpp>

namespace chess {

namespace x3 = boost::spirit::x3;

struct node_tag;
struct square_tag;

const x3::rule<class fen_rule> fen_rule ="fen";
const x3::rule<class rows_rule> rows_rule ="rows";
const x3::rule<class row_rule> row_rule ="row";
const x3::rule<class piece_rule> piece_rule ="piece";
const x3::rule<class empty_rule> empty_rule ="empty";

const auto fen_action = [](auto& ctx)
{
	node_t& node = x3::get<node_tag>(ctx);
	node.attack_white = attack_generator::generate<white_tag>(node);
	node.attack_black = attack_generator::generate<black_tag>(node);
};

const auto rows_action = [](auto& ctx)
{
};

const auto row_action = [](auto& ctx)
{
	square_t& square = x3::get<square_tag>(ctx);
//	node_t& node = x3::get<node_tag>(ctx);
////	const char ch = x3::_attr(ctx);
//

	square -= 16;
};

//template <piece_t piece>
struct foo
{
	template <piece_t piece>
	static void set(node_t& node, const square_t square)
	{
		node.pieces.at(square) = piece;
		node.hash ^= piece_square_hash.at(piece).at(square);
		set_mask<piece>(node, board_of(square));
	}

private:
	template <piece_t piece>
	static void set_mask(node_t& node, const board_t mask);
};

template <>
void foo::set<no_piece>(node_t& node, const square_t square)
{
	node.pieces.at(square) = no_piece;
}

template <>
void foo::set<K>(node_t& node, const square_t square)
{
	const board_t mask = board_of(square);
	node.occupy_white |= mask;
	node.king_white = square;
	node.pieces.at(square) = K;
	node.hash ^= piece_square_hash.at(K).at(square);
}

template <>
void foo::set<k>(node_t& node, const square_t square)
{
	const board_t mask = board_of(square);
	node.occupy_black |= mask;
	node.king_black = square;
	node.pieces.at(square) = k;
	node.hash ^= piece_square_hash.at(k).at(square);
}

template <>
void foo::set_mask<Q>(node_t& node, const board_t mask)
{
	node.occupy_white |= mask;
	node.occupy_rook_queen |= mask;
	node.occupy_bishop_queen |= mask;
	node.score += 900;
}

template <>
void foo::set_mask<q>(node_t& node, const board_t mask)
{
	node.occupy_black |= mask;
	node.occupy_rook_queen |= mask;
	node.occupy_bishop_queen |= mask;
	node.score -= 900;
}

template <>
void foo::set_mask<R>(node_t& node, const board_t mask)
{
	node.occupy_white |= mask;
	node.occupy_rook_queen |= mask;
	node.score += 500;
}

template <>
void foo::set_mask<r>(node_t& node, const board_t mask)
{
	node.occupy_black |= mask;
	node.occupy_rook_queen |= mask;
	node.score -= 500;
}

template <>
void foo::set_mask<B>(node_t& node, const board_t mask)
{
	node.occupy_white |= mask;
	node.occupy_bishop_queen |= mask;
	node.score += 300;
}

template <>
void foo::set_mask<b>(node_t& node, const board_t mask)
{
	node.occupy_black |= mask;
	node.occupy_bishop_queen |= mask;
	node.score -= 300;
}

template <>
void foo::set_mask<N>(node_t& node, const board_t mask)
{
	node.occupy_white |= mask;
	node.occupy_knight |= mask;
	node.score += 300;
}

template <>
void foo::set_mask<n>(node_t& node, const board_t mask)
{
	node.occupy_black |= mask;
	node.occupy_knight |= mask;
	node.score -= 300;
}

template <>
void foo::set_mask<P>(node_t& node, const board_t mask)
{
	node.occupy_white |= mask;
	node.occupy_pawn |= mask;
	node.score += 100;
}

template <>
void foo::set_mask<p>(node_t& node, const board_t mask)
{
	node.occupy_black |= mask;
	node.occupy_pawn |= mask;
	node.score -= 100;
}

const auto piece_action = [](auto& ctx)
{
	static const std::unordered_map<char, std::function<void (node_t&, const square_t)>> MAP
	{
		{'K', foo::set<K>}, {'Q', foo::set<Q>}, {'R', foo::set<R>}, {'B', foo::set<B>}, {'N', foo::set<N>}, {'P', foo::set<P>},
		{'k', foo::set<k>}, {'q', foo::set<q>}, {'r', foo::set<r>}, {'b', foo::set<b>}, {'n', foo::set<n>}, {'p', foo::set<p>}
	};

	MAP.at(x3::_attr(ctx))(x3::get<node_tag>(ctx), x3::get<square_tag>(ctx)++);
};

const auto empty_action = [](auto& ctx)
{
	square_t& square = x3::get<square_tag>(ctx);
	node_t& node = x3::get<node_tag>(ctx);
	const char ch = x3::_attr(ctx);

//	std::cout << &node << " : " << int(square) << " = " << '-' << std::endl;

	for (char i = 0; i < (ch - '0'); ++i)
		node.pieces.at(square++) = no_piece;
};

const auto fen_rule_def = rows_rule [fen_action];
const auto rows_rule_def = (row_rule % x3::lit('/'));// [rows_action];
const auto row_rule_def = (+(piece_rule | empty_rule)) [row_action];
const auto piece_rule_def = x3::char_("KQRBNPkqrbnp") [piece_action];
const auto empty_rule_def = x3::char_("1-8") [empty_action];

BOOST_SPIRIT_DEFINE(fen_rule, rows_rule, row_rule, piece_rule, empty_rule);

void parse(std::istream& stream, node_t& node)
{
	typedef boost::spirit::istream_iterator iterator_t;

	stream.unsetf(std::ios::skipws);

	memset(&node, 0, sizeof(node_t));
	square_t square = a8;

	auto rule = x3::with<node_tag>(std::ref(node)) [ x3::with<square_tag>(std::ref(square)) [x3::eps > fen_rule] ];
	iterator_t iterator(stream);

	const bool ok = x3::parse(iterator, iterator_t(), rule);

	if (!ok || iterator != iterator_t())
		throw std::runtime_error("Parsing failed.");

	std::cout << int(square) << std::endl;
}

}
*/
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
//	stream << "Color = " << node.color() << endl;
//	stream << "EnPassant = ";
//	stream << char('a' + file_of(node.square_en_passant()));
//	stream << char('1' + rank_of(node.square_en_passant()));
//	stream << endl;
	return stream;
}



istream&
operator>>(istream& stream, chess::node_t& node)
{
//	parse(stream, node);
	return stream;
}

}
