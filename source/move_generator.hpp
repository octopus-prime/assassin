/*
 * move_generator.hpp
 *
 *  Created on: 07.08.2016
 *      Author: mike
 */

#pragma once

#include "history_table.hpp"
#include "killer_table.hpp"
#include "move.hpp"
#include "node.hpp"

namespace chess {

typedef std::array<move_t, 256> moves_t;

template <typename moves_tag>
class move_generator {
  struct eval_t {
    float good;
    score_t score;
    std::uint8_t killer;
  };

  typedef std::array<eval_t, moves_t().size()> evals_t;

 public:
  move_generator(const node_t& node, const history_table_t& h_table,
                 const history_table_t& b_table) noexcept;
  move_generator(const node_t& node, const history_table_t& h_table,
                 const history_table_t& b_table,
                 const killer_table_t::entry_t& k_entry1,
                 const killer_table_t::entry_t& k_entry2) noexcept;
  move_generator(const node_t& node, const history_table_t& h_table,
                 const history_table_t& b_table,
                 const killer_table_t::entry_t& k_entry1,
                 const killer_table_t::entry_t& k_entry2,
                 const killer_table_t::entry_t& k_entry3) noexcept;

  constexpr moves_t::const_iterator begin() const noexcept {
    return _moves.begin();
  }

  constexpr moves_t::const_iterator end() const noexcept { return _end; }

  constexpr size_t size() const noexcept {
    return std::distance(begin(), end());
  }

  constexpr score_t score(const move_t move) const noexcept {
    return _evals[move.metadata.index].score;
  }

  constexpr std::uint8_t killer(const move_t move) const noexcept {
    return _evals[move.metadata.index].killer;
  }

 protected:
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept;

  template <typename color_tag>
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept;

 private:
  moves_t _moves;
  moves_t::iterator _end;
  evals_t _evals;
};

}  // namespace chess
