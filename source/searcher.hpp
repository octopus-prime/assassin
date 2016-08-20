/*
 * searcher.hpp
 *
 *  Created on: 20.08.2016
 *      Author: mike
 */

#pragma once

#include "transposition_table.hpp"
#include "history_table.hpp"
#include "killer_table.hpp"
#include <chrono>
#include <functional>

namespace chess {

typedef std::chrono::high_resolution_clock clock;
typedef std::function<void (const std::uint_fast8_t iteration, const std::uint_fast8_t max_height, const score_t score, const clock::duration duration, const std::size_t count, const std::string& pv)> report_t;

class searcher
{
public:
	searcher(transposition_table_t& t_table, const report_t& report) noexcept;

	score_t
	operator()(const node_t& node, const std::uint_fast8_t depth);

	void
	stop();

protected:
	score_t
	search(const node_t& node, const score_t alpha, const score_t beta, std::uint_fast8_t depth, const std::uint_fast8_t height, const move_t moved);

	score_t
	search(const node_t& node, const score_t alpha, const score_t beta);

	static constexpr bool
	test_check(const node_t& node, const color_t color) noexcept;

	static std::uint_fast8_t
	count_repetition(const node_t& leaf) noexcept;

	template <typename color_tag>
	static bool
	try_null(const node_t& node) noexcept;

	static bool
	try_null(const node_t& node) noexcept;

	std::string
	get_pv(node_t node) const;

private:
	report_t _report;
	transposition_table_t& _t_table;
	history_table_t _h_table;
	history_table_t _b_table;
	killer_table_t _k_table;
	std::atomic_uint_fast64_t _count;
	std::atomic_uint_fast8_t _height;
	std::atomic_bool _stop;
	clock::time_point _start;
};

}
