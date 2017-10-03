/*
 * move.hpp
 *
 *  Created on: 21.05.2016
 *      Author: mike_gresens
 */

#pragma once

#include "square.hpp"

namespace chess {

struct move_t {
  enum promotion_t : std::uint8_t {no, N, B, R, Q};

  square_t from;
  square_t to;
  promotion_t promotion;
  union {
    std::uint8_t index;
    std::uint8_t check;
  } metadata;
};

constexpr bool operator==(const move_t move1, const move_t move2) noexcept {
  return move1.from == move2.from && move1.to == move2.to &&
         move1.promotion == move2.promotion;
}

constexpr bool operator!=(const move_t move1, const move_t move2) noexcept {
  return !(move1 == move2);
}

}  // namespace chess
