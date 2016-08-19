/*
 * evaluator.cpp
 *
 *  Created on: 18.08.2016
 *      Author: mike
 */

#include "evaluator.hpp"
#include "attacker.hpp"

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
	static constexpr score_t weight = 2;

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
struct evaluator<interaction_defend_tag>
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
	static constexpr score_t weight = 50;

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
	static constexpr score_t weight = 25;

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
	static constexpr score_t weight = 15;

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
		constexpr std::array<board_t, 8> files {Fa, Fb, Fc, Fd, Fe, Ff, Fg, Fh};
		for (board_t file : files)
		{
			const board_t pawns = node.occupy<pawn_tag, other_tag>() & file;
			const score_t count = popcnt(pawns);
			score += std::max(0, count - 1);
		}
		return score;
	}
};

}

score_t
evaluator::evaluate(const node_t& node) noexcept
{
	score_t score = node.score();
	score += evaluate<detail::mobility_occupy_tag>(node);
	score += evaluate<detail::interaction_attack_tag>(node);
	score += evaluate<detail::interaction_defend_tag>(node);
	score += evaluate<detail::center_occupy_tag>(node);
	score += evaluate<detail::center_attack_tag>(node);
	score += evaluate<detail::center2_occupy_tag>(node);
	score += evaluate<detail::center2_attack_tag>(node);
	score += evaluate<detail::king_attack_tag>(node);
	score += evaluate<detail::king_defend_tag>(node);
	score += evaluate<detail::pawn_defend_tag>(node);
	score += evaluate<detail::pawn1_attack_tag>(node);
	score += evaluate<detail::pawn2_attack_tag>(node);
	score += evaluate<detail::pawn3_attack_tag>(node);
	score += evaluate<detail::fork_attack_tag>(node);
	score += evaluate<detail::pawn_connect_tag>(node);
	score += evaluate<detail::pawn_blocked_tag>(node);
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
