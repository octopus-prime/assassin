/*
 * evaluator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

namespace chess {
namespace detail {

struct mobility_tag;
struct attack_tag;
struct defend_tag;
struct center_tag;

template <typename feature_tag>
struct evaluator;

template <>
struct evaluator<mobility_tag>
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
struct evaluator<attack_tag>
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
struct evaluator<defend_tag>
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
struct evaluator<center_tag>
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

}

struct evaluator
{
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		score_t score = node.score();
		score += evaluate<detail::mobility_tag>(node);
		score += evaluate<detail::attack_tag>(node);
		score += evaluate<detail::defend_tag>(node);
		score += evaluate<detail::center_tag>(node);
		return score * node.color();
	}

private:
	template <typename feature_tag>
	static constexpr score_t
	evaluate(const node_t& node) noexcept
	{
		const score_t white = detail::evaluator<feature_tag>::template evaluate<white_tag>(node);
		const score_t black = detail::evaluator<feature_tag>::template evaluate<black_tag>(node);
		const score_t weight = detail::evaluator<feature_tag>::weight;
		return (white - black) * weight;
	}
};

}
