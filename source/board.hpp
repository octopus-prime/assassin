/*
 * board.hpp
 *
 *  Created on: 11.04.2016
 *      Author: mike_gresens
 */

#pragma once

#include "square.hpp"
#include <cstdint>
#include <bitset>
#include <array>
#include <iostream>
#include <x86intrin.h>
#include <boost/predef/hardware/simd.h>

static_assert(BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_AVX2_VERSION, "Minimum AVX2 required.");

#include <immintrin.h>

namespace chess {

typedef std::uint64_t board_t;

constexpr board_t
board_of(const square_t square) noexcept
{
	return 1UL << square;
}

class bsf
{
public:
	class iterator
	{
	public:
		constexpr
		iterator(const board_t board) noexcept
		:
			_board(board)
		{
		}

		constexpr void
		operator++() const noexcept
		{
		}

		constexpr bool
		operator!=(const iterator) const noexcept
		{
			return _board != 0UL;
		}

		square_t
		operator*() noexcept
		{
			const square_t square = __bsfq(_board);
			_board ^= board_of(square);
			return square;
		}

	private:
		board_t _board;
	};

	constexpr
	bsf(const board_t board) noexcept
	:
		_board(board)
	{
	}

	constexpr iterator
	begin() const noexcept
	{
		return iterator(_board);
	}

	constexpr iterator
	end() const noexcept
	{
		return iterator(0UL);
	}

private:
	board_t _board;
};

inline size_t
popcnt(const board_t board) noexcept
{
	return __popcntq(board);
}

enum board : board_t
{
	A1 = board_of(a1), B1 = board_of(b1), C1 = board_of(c1), D1 = board_of(d1), E1 = board_of(e1), F1 = board_of(f1), G1 = board_of(g1), H1 = board_of(h1),
	A2 = board_of(a2), B2 = board_of(b2), C2 = board_of(c2), D2 = board_of(d2), E2 = board_of(e2), F2 = board_of(f2), G2 = board_of(g2), H2 = board_of(h2),
	A3 = board_of(a3), B3 = board_of(b3), C3 = board_of(c3), D3 = board_of(d3), E3 = board_of(e3), F3 = board_of(f3), G3 = board_of(g3), H3 = board_of(h3),
	A4 = board_of(a4), B4 = board_of(b4), C4 = board_of(c4), D4 = board_of(d4), E4 = board_of(e4), F4 = board_of(f4), G4 = board_of(g4), H4 = board_of(h4),
	A5 = board_of(a5), B5 = board_of(b5), C5 = board_of(c5), D5 = board_of(d5), E5 = board_of(e5), F5 = board_of(f5), G5 = board_of(g5), H5 = board_of(h5),
	A6 = board_of(a6), B6 = board_of(b6), C6 = board_of(c6), D6 = board_of(d6), E6 = board_of(e6), F6 = board_of(f6), G6 = board_of(g6), H6 = board_of(h6),
	A7 = board_of(a7), B7 = board_of(b7), C7 = board_of(c7), D7 = board_of(d7), E7 = board_of(e7), F7 = board_of(f7), G7 = board_of(g7), H7 = board_of(h7),
	A8 = board_of(a8), B8 = board_of(b8), C8 = board_of(c8), D8 = board_of(d8), E8 = board_of(e8), F8 = board_of(f8), G8 = board_of(g8), H8 = board_of(h8),

	Fa = A1 | A2 | A3 | A4 | A5 | A6 | A7 | A8,
	Fb = Fa << 1,
	Fc = Fa << 2,
	Fd = Fa << 3,
	Fe = Fa << 4,
	Ff = Fa << 5,
	Fg = Fa << 6,
	Fh = Fa << 7,

	R1 = A1 | B1 | C1 | D1 | E1 | F1 | G1 | H1,
	R2 = R1 << 8,
	R3 = R1 << 16,
	R4 = R1 << 24,
	R5 = R1 << 32,
	R6 = R1 << 40,
	R7 = R1 << 48,
	R8 = R1 << 56,
};

typedef __v4du board4_t;

constexpr inline board4_t
make_board4(const board_t board) noexcept
{
	return board4_t {board, board, board, board};
}

}

namespace std {

inline ostream&
operator<<(ostream& stream, const bitset<64>& board)
{
	using namespace chess;
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

}
