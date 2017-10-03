/*
 * evaluator.cpp
 *
 *  Created on: 18.08.2016
 *      Author: mike
 */

#include "evaluator.hpp"
#include "attacker.hpp"
#include "analyzer.hpp"
#include "exchanger.hpp"
#include <algorithm>



#include "io.hpp"

namespace chess {
namespace detail {

struct mobility_occupy_tag;
struct interaction_attack_tag;
struct interaction_defend_tag;
struct center_occupy_tag;
struct center_attack_tag;
struct center2_occupy_tag;
struct center2_attack_tag;
struct king_attack_tag;
struct king_defend_tag;
struct pawn_defend_tag;
struct pawn1_attack_tag;
struct pawn2_attack_tag;
struct pawn3_attack_tag;
struct fork_attack_tag;
struct pawn_connect_tag;
struct pawn_blocked_tag;

template <typename feature_tag>
struct evaluator;

template <>
struct evaluator<mobility_occupy_tag>
{
	static constexpr score_t weight = 1;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		return popcnt(node.attack<color_tag>());
	}
};

template <>
struct evaluator<interaction_attack_tag>
{
	static constexpr score_t weight = 2;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		return popcnt(node.attack<color_tag>() & node.occupy<other_tag>());
	}
};

template <>
struct evaluator<interaction_defend_tag>
{
	static constexpr score_t weight = 1;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		return popcnt(node.attack<color_tag>() & node.occupy<color_tag>());
	}
};

template <>
struct evaluator<center_occupy_tag>
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

template <>
struct evaluator<center_attack_tag>
{
	static constexpr score_t weight = 5;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		constexpr board_t center = D4 | E4 | D5 | E5;
		return popcnt(node.attack<color_tag>() & center);
	}
};

template <>
struct evaluator<center2_occupy_tag>
{
	static constexpr score_t weight = 5;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		constexpr board_t center = C3 | D3 | E3 | F3 | C4 | F4 | C5 | F5 | C6 | D6 | E6 | F6;
		return popcnt(node.occupy<color_tag>() & center);
	}
};

template <>
struct evaluator<center2_attack_tag>
{
	static constexpr score_t weight = 3;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		constexpr board_t center = C3 | D3 | E3 | F3 | C4 | F4 | C5 | F5 | C6 | D6 | E6 | F6;
		return popcnt(node.attack<color_tag>() & center);
	}
};

template <>
struct evaluator<king_attack_tag>
{
	static constexpr score_t weight = 10;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		return popcnt(node.attack<color_tag>() & king);
	}
};

template <>
struct evaluator<king_defend_tag>
{
	static constexpr score_t weight = 5;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		const board_t king = attacker<king_tag, color_tag>::attack(node.occupy<king_tag, color_tag>());
		return popcnt(node.attack<color_tag>() & king);
	}
};

template <>
struct evaluator<pawn_defend_tag>
{
	static constexpr score_t weight = 3;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		const board_t king = attacker<king_tag, color_tag>::attack(node.occupy<king_tag, color_tag>());
		return popcnt(node.occupy<pawn_tag,  color_tag>() & king);
	}
};

template <>
struct evaluator<pawn1_attack_tag>
{
	static constexpr score_t weight = 10;

	template <typename color_tag>
	static score_t
	evaluate(const node_t& node) noexcept;
};

template <>
inline score_t
evaluator<pawn1_attack_tag>::evaluate<white_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, white_tag>() & R5);
}

template <>
inline score_t
evaluator<pawn1_attack_tag>::evaluate<black_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, black_tag>() & R4);
}

template <>
struct evaluator<pawn2_attack_tag>
{
	static constexpr score_t weight = 50;

	template <typename color_tag>
	static score_t
	evaluate(const node_t& node) noexcept;
};

template <>
inline score_t
evaluator<pawn2_attack_tag>::evaluate<white_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, white_tag>() & R6);
}

template <>
inline score_t
evaluator<pawn2_attack_tag>::evaluate<black_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, black_tag>() & R3);
}

template <>
struct evaluator<pawn3_attack_tag>
{
	static constexpr score_t weight = 100;

	template <typename color_tag>
	static score_t
	evaluate(const node_t& node) noexcept;
};

template <>
inline score_t
evaluator<pawn3_attack_tag>::evaluate<white_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, white_tag>() & R7);
}

template <>
inline score_t
evaluator<pawn3_attack_tag>::evaluate<black_tag>(const node_t& node) noexcept
{
	return popcnt(node.occupy<pawn_tag, black_tag>() & R2);
}

template <>
struct evaluator<fork_attack_tag>
{
	static constexpr score_t weight = 33;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		const board_t knight = attacker<knight_tag, color_tag>::attack(node.occupy<knight_tag, color_tag>());
		return popcnt((node.occupy<king_tag, other_tag>() | node.occupy<rook_queen_tag, other_tag>()) & knight);
	}
};

template <>
struct evaluator<pawn_connect_tag>
{
	static constexpr score_t weight = 20;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		score_t score = 0;
		for (const square_t pawn : bsf(node.occupy<pawn_tag, color_tag>()))
			score += popcnt(attacker<king_tag, color_tag>::attack(board_of(pawn)));
		return score;
	}
};

template <>
struct evaluator<pawn_blocked_tag>
{
	static constexpr score_t weight = 50;

	template <typename color_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		score_t score = 0;
		constexpr std::array<board_t, 8> files {{Fa, Fb, Fc, Fd, Fe, Ff, Fg, Fh}};
		for (board_t file : files)
		{
			const board_t pawns = node.occupy<pawn_tag, other_tag>() & file;
			const score_t count = popcnt(pawns);
			score += std::max(0, count - 1);
		}
		return score;
	}
};

typedef std::array<std::array<score_t, 64>, 13> scores_t;

static std::array<score_t, 64>
reverse(const std::array<score_t, 64>& black) noexcept
{
	std::array<score_t, 64> white;
	std::reverse_copy(black.begin(), black.end(), white.begin());
	return white;
}

constexpr std::array<score_t, 64> scoresKm
{{
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-30,-40,-40,-50,-50,-40,-40,-30,
	-20,-30,-30,-40,-40,-30,-30,-20,
	-10,-20,-20,-20,-20,-20,-20,-10,
	 20, 20,  0,  0,  0,  0, 20, 20,
	 20, 30, 10,  0,  0, 10, 30, 20
}};
constexpr std::array<score_t, 64> scoresKe
{{
	-50,-40,-30,-20,-20,-30,-40,-50,
	-30,-20,-10,  0,  0,-10,-20,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 30, 40, 40, 30,-10,-30,
	-30,-10, 20, 30, 30, 20,-10,-30,
	-30,-30,  0,  0,  0,  0,-30,-30,
	-50,-30,-30,-30,-30,-30,-30,-50
}};

constexpr std::array<score_t, 64> scoresQ
{{
	-20,-10,-10, -5, -5,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5,  5,  5,  5,  0,-10,
	 -5,  0,  5,  5,  5,  5,  0, -5,
	  0,  0,  5,  5,  5,  5,  0, -5,
	-10,  5,  5,  5,  5,  5,  0,-10,
	-10,  0,  5,  0,  0,  0,  0,-10,
	-20,-10,-10, -5, -5,-10,-10,-20
}};
constexpr std::array<score_t, 64> scoresR
{{
	  0,  0,  0,  0,  0,  0,  0,  0,
	  5, 10, 10, 10, 10, 10, 10,  5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	 -5,  0,  0,  0,  0,  0,  0, -5,
	  0,  0,  0,  5,  5,  0,  0,  0
}};
constexpr std::array<score_t, 64> scoresB
{{
	-20,-10,-10,-10,-10,-10,-10,-20,
	-10,  0,  0,  0,  0,  0,  0,-10,
	-10,  0,  5, 10, 10,  5,  0,-10,
	-10,  5,  5, 10, 10,  5,  5,-10,
	-10,  0, 10, 10, 10, 10,  0,-10,
	-10, 10, 10, 10, 10, 10, 10,-10,
	-10,  5,  0,  0,  0,  0,  5,-10,
	-20,-10,-10,-10,-10,-10,-10,-20,
}};
constexpr std::array<score_t, 64> scoresN
{{
	-50,-40,-30,-30,-30,-30,-40,-50,
	-40,-20,  0,  0,  0,  0,-20,-40,
	-30,  0, 10, 15, 15, 10,  0,-30,
	-30,  5, 15, 20, 20, 15,  5,-30,
	-30,  0, 15, 20, 20, 15,  0,-30,
	-30,  5, 10, 15, 15, 10,  5,-30,
	-40,-20,  0,  5,  5,  0,-20,-40,
	-50,-40,-30,-30,-30,-30,-40,-50,
}};
constexpr std::array<score_t, 64> scoresP
{{
	 0,  0,  0,  0,  0,  0,  0,  0,
	50, 50, 50, 50, 50, 50, 50, 50,
	10, 10, 20, 30, 30, 20, 10, 10,
	 5,  5, 10, 25, 25, 10,  5,  5,
	 0,  0,  0, 20, 20,  0,  0,  0,
	 5, -5,-10,  0,  0,-10, -5,  5,
	 5, 10, 10,-20,-20, 10, 10,  5,
	 0,  0,  0,  0,  0,  0,  0,  0
}};

template <typename piece_tag, typename color_tag>
struct evaluator2;

template <typename color_tag>
struct evaluator2<pawn_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
		typedef pawn_analyser_t<color_tag> analyser_t;
		const analyser_t analyze(node);

		score_t score = 0;
		for (const square_t square : bsf(node.occupy<pawn_tag, color_tag>()))
		{
			score += scores[square];
			score += bonus[rank_of(square)] / 2;
//			score += evaluate_free(node, square) * 30;
			score += evaluate_king(node, square, king) * 1;
//			score -= evaluate_multi(node, square) * 25;
//			score -= evaluate_iso(node, square) * 20;

			const auto properties = analyze(square);
			score += analyser_t::is_passed(properties) * (30 + bonus[rank_of(square)] /* * (node.king<other_tag>() != square + 8)*/);
			score += analyser_t::is_supported(properties) * 7;
			score -= analyser_t::is_doubled(properties) * 25;
			score -= analyser_t::is_isolated(properties) * 20;
			score -= analyser_t::is_backwarded(properties) * 15;

//			score -= evaluate_back(node, square) * 15;
		}
		return score;
	}

protected:
	static score_t
	evaluate_multi(const node_t& node, const square_t square) noexcept
	{
		const board_t file = Fa << file_of(square);
		return popcnt(node.occupy<pawn_tag, color_tag>() & file);
	}

	static score_t
	evaluate_iso(const node_t& node, const square_t square) noexcept
	{
		const auto foo = file_of(square);
		if (foo > 0)
		{
			const board_t file = Fa << (foo - 1);
			if (node.occupy<pawn_tag, color_tag>() & file)
				return false;
		}
		if (foo < 7)
		{
			const board_t file = Fa << (foo + 1);
			if (node.occupy<pawn_tag, color_tag>() & file)
				return false;
		}
		return true;
	}

	static score_t
	evaluate_free(const node_t& node, const square_t square) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		const auto foo = file_of(square);
		if (foo > 0)
		{
			const board_t file = Fa << (foo - 1);
			if (node.occupy<pawn_tag, other_tag>() & file)
				return false;
		}
		if (foo < 7)
		{
			const board_t file = Fa << (foo + 1);
			if (node.occupy<pawn_tag, other_tag>() & file)
				return false;
		}
		return true;
	}

	static score_t
	evaluate_king(const node_t& node, const square_t square, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
//		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		const board_t pawn = attacker<pawn_tag, color_tag>::attack(board_of(square));
		return popcnt(king & pawn) * 10;
	}

//	static score_t
//	evaluate_back(const node_t& node, const square_t square) noexcept;

private:
	static const std::array<score_t, 64> scores;
	static const std::array<score_t, 8> bonus;
};

//template <>
//inline score_t
//evaluator2<pawn_tag, white_tag>::evaluate_back(const node_t& node, const square_t square) noexcept
//{
//	score_t result = 0;
//	if (node[square + 8] == no_piece)
//	{
//		const score_t score = exchanger_t(node)(move_t {square, square_t(square + 8)});
//		if (score < 0)
//			++result;
//
//		if (rank_of(square) == 1 && node[square + 16] == no_piece)
//		{
//			const score_t score = exchanger_t(node)(move_t {square, square_t(square + 16)});
//			if (score < 0)
//				++result;
//		}
//	}
//	return result;
//}
//
//template <>
//inline score_t
//evaluator2<pawn_tag, black_tag>::evaluate_back(const node_t& node, const square_t square) noexcept
//{
//	score_t result = 0;
//	if (node[square - 8] == no_piece)
//	{
//		const score_t score = exchanger_t(node)(move_t {square, square_t(square - 8)});
//		if (score < 0)
//			++result;
//
//		if (rank_of(square) == 6 && node[square - 16] == no_piece)
//		{
//			const score_t score = exchanger_t(node)(move_t {square, square_t(square - 16)});
//			if (score < 0)
//				++result;
//		}
//	}
//	return result;
//}

template <>
const std::array<score_t, 64>
evaluator2<pawn_tag, white_tag>::scores = reverse(scoresP);

template <>
const std::array<score_t, 64>
evaluator2<pawn_tag, black_tag>::scores = scoresP;

template <>
const std::array<score_t, 8>
evaluator2<pawn_tag, white_tag>::bonus {{0, 0, 0, 0, 0, 10, 50, 0}};

template <>
const std::array<score_t, 8>
evaluator2<pawn_tag, black_tag>::bonus {{0, 50, 10, 0, 0, 0, 0, 0}};

template <typename color_tag>
struct evaluator2<knight_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
		score_t score = 0;
		for (const square_t square : bsf(node.occupy<knight_tag, color_tag>()))
		{
			score += scores[square];
			score += evaluate_fork(node, square) * 25;
			score += evaluate_king(node, square, king) * 3;
		}
		return score;
	}

protected:
	static score_t
	evaluate_fork(const node_t& node, const square_t square) noexcept
	{
		typedef typename color_traits<color_tag>::other other_tag;
		const board_t knight = attacker<knight_tag, color_tag>::attack(board_of(square));
		return popcnt((node.occupy<king_tag, other_tag>() | node.occupy<rook_queen_tag, other_tag>()) & knight);
	}

	static score_t
	evaluate_king(const node_t& node, const square_t square, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
//		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		const board_t knight = attacker<knight_tag, color_tag>::attack(board_of(square));
		return popcnt(king & knight) * 10;
	}

private:
	static const std::array<score_t, 64> scores;
};

template <>
const std::array<score_t, 64>
evaluator2<knight_tag, white_tag>::scores = reverse(scoresN);

template <>
const std::array<score_t, 64>
evaluator2<knight_tag, black_tag>::scores = scoresN;

template <typename color_tag>
struct evaluator2<bishop_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
		score_t score = 0;
		for (const square_t square : bsf(node.occupy<bishop_tag, color_tag>()))
		{
			score += scores[square];
			score += evaluate_king(node, square, king) * 3;
		}
		return score;
	}

protected:
	static score_t
	evaluate_king(const node_t& node, const square_t square, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
//		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		const board_t bishop1 = attacker<bishop_queen_tag, color_tag>::attack(board_of(square), 0UL);
		const board_t bishop2 = attacker<bishop_queen_tag, color_tag>::attack(board_of(square), node.occupy());
		return popcnt(king & bishop1) + popcnt(king & bishop2) * 10;
//		return file_of(square) == file_of(node.king<other_tag>()) || rank_of(square) == rank_of(node.king<other_tag>());
	}

private:
	static const std::array<score_t, 64> scores;
};

template <>
const std::array<score_t, 64>
evaluator2<bishop_tag, white_tag>::scores = reverse(scoresB);

template <>
const std::array<score_t, 64>
evaluator2<bishop_tag, black_tag>::scores = scoresB;

template <typename color_tag>
struct evaluator2<rook_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
		score_t score = 0;
		for (const square_t square : bsf(node.occupy<rook_tag, color_tag>()))
		{
			score += scores[square];
			score += evaluate_open(node, square) * 10;
			score += evaluate_king(node, square, king) * 5;
		}
		return score;
	}

protected:
	static score_t
	evaluate_open(const node_t& node, const square_t square) noexcept
	{
		const board_t file = Fa << file_of(square);
		return (node.occupy<pawn_tag>() & file) == 0;
	}

	static score_t
	evaluate_king(const node_t& node, const square_t square, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
//		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		const board_t rook1 = attacker<rook_queen_tag, color_tag>::attack(board_of(square), 0UL);
		const board_t rook2 = attacker<rook_queen_tag, color_tag>::attack(board_of(square), node.occupy());
		return popcnt(king & rook1) + popcnt(king & rook2) * 10;
//		return file_of(square) == file_of(node.king<other_tag>()) || rank_of(square) == rank_of(node.king<other_tag>());
	}

private:
	static const std::array<score_t, 64> scores;
};

template <>
const std::array<score_t, 64>
evaluator2<rook_tag, white_tag>::scores = reverse(scoresR);

template <>
const std::array<score_t, 64>
evaluator2<rook_tag, black_tag>::scores = scoresR;

template <typename color_tag>
struct evaluator2<queen_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
		score_t score = 0;
		for (const square_t square : bsf(node.occupy<queen_tag, color_tag>()))
		{
			score += scores[square];
			score += evaluate_king(node, square, king) * 9;
		}
		return score;
	}

protected:
	static score_t
	evaluate_king(const node_t& node, const square_t square, const board_t king) noexcept
	{
//		typedef typename color_traits<color_tag>::other other_tag;
//		const board_t king = attacker<king_tag, other_tag>::attack(node.occupy<king_tag, other_tag>()) | node.occupy<king_tag, other_tag>();
		const board_t queen1 = attacker<sliding_tag, color_tag>::attack(board_of(square), board_of(square), 0UL);
		const board_t queen2 = attacker<sliding_tag, color_tag>::attack(board_of(square), board_of(square), node.occupy());
		return popcnt(king & queen1) + popcnt(king & queen2) * 10;
//		return file_of(square) == file_of(node.king<other_tag>()) || rank_of(square) == rank_of(node.king<other_tag>());
	}

protected:

private:
	static const std::array<score_t, 64> scores;
};

template <>
const std::array<score_t, 64>
evaluator2<queen_tag, white_tag>::scores = reverse(scoresQ);

template <>
const std::array<score_t, 64>
evaluator2<queen_tag, black_tag>::scores = scoresQ;

template <typename color_tag>
struct evaluator2<king_tag, color_tag>
{
	static score_t
	evaluate(const node_t& node, const board_t king) noexcept
	{
		score_t score = 0;
		const square_t square = node.king<color_tag>();
		score += scores[square];
		return score;
	}

protected:

private:
	static const std::array<score_t, 64> scores;
};

template <>
const std::array<score_t, 64>
evaluator2<king_tag, white_tag>::scores = reverse(scoresKm);

template <>
std::array<score_t, 64>
const evaluator2<king_tag, black_tag>::scores = scoresKm;

template <typename piece_tag>
static score_t
evaluate2(const node_t& node, const board_t king_w, const board_t king_b) noexcept
{
	return evaluator2<piece_tag, white_tag>::evaluate(node, king_b) - evaluator2<piece_tag, black_tag>::evaluate(node, king_w);
}

}

score_t
evaluator::evaluate(const node_t& node) noexcept
{
	score_t score = node.score();
	score += evaluate<detail::mobility_occupy_tag>(node);
	score += evaluate<detail::interaction_attack_tag>(node);
//	score += evaluate<detail::interaction_defend_tag>(node);
//	score += evaluate<detail::center_occupy_tag>(node);
//	score += evaluate<detail::center_attack_tag>(node);
//	score += evaluate<detail::center2_occupy_tag>(node);
//	score += evaluate<detail::center2_attack_tag>(node);
	score += evaluate<detail::king_attack_tag>(node);
//	score += evaluate<detail::king_defend_tag>(node);
//	score += evaluate<detail::pawn_defend_tag>(node);
//	score += evaluate<detail::pawn1_attack_tag>(node);
//	score += evaluate<detail::pawn2_attack_tag>(node);
//	score += evaluate<detail::pawn3_attack_tag>(node);
//	score += evaluate<detail::fork_attack_tag>(node);
//	score += evaluate<detail::pawn_connect_tag>(node);
//	score += evaluate<detail::pawn_blocked_tag>(node);

//	for (int i = 0; i < 64; i++)
//		score += detail::scores[node[i]][i];

	board_t king_w = detail::attacker<king_tag, white_tag>::attack(node.occupy<king_tag, white_tag>()) | node.occupy<king_tag, white_tag>();
	board_t king_b = detail::attacker<king_tag, black_tag>::attack(node.occupy<king_tag, black_tag>()) | node.occupy<king_tag, black_tag>();

	king_w = king_w | king_w << 8;
	king_b = king_b | king_b >> 8;

	score += detail::evaluate2<king_tag>(node, king_w, king_b);
	score += detail::evaluate2<queen_tag>(node, king_w, king_b);
	score += detail::evaluate2<rook_tag>(node, king_w, king_b);
	score += detail::evaluate2<bishop_tag>(node, king_w, king_b);
	score += detail::evaluate2<knight_tag>(node, king_w, king_b);
	score += detail::evaluate2<pawn_tag>(node, king_w, king_b);
	return score * node.color();
}

template <typename feature_tag>
constexpr score_t
evaluator::evaluate(const node_t& node) noexcept
{
	const score_t white = detail::evaluator<feature_tag>::template evaluate<white_tag>(node);
	const score_t black = detail::evaluator<feature_tag>::template evaluate<black_tag>(node);
	const score_t weight = detail::evaluator<feature_tag>::weight;
	return (white - black) * weight;
}

}
