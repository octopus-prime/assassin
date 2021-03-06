/*
 * move_generator.cpp
 *
 *  Created on: 23.08.2016
 *      Author: mike
 */

#include "move_generator.hpp"
#include <algorithm>
#include "attacker.hpp"
#include "exchanger.hpp"

namespace chess {
namespace detail {

template <typename moves_tag, typename color_tag>
struct masker;

template <typename color_tag>
struct masker<all_tag, color_tag> {
  static constexpr board_t mask(const node_t& node) noexcept {
    return ~node.occupy<color_tag>();
  }
};

template <typename color_tag>
struct masker<passive_tag, color_tag> {
  static constexpr board_t mask(const node_t& node) noexcept {
    return ~node.occupy();
  }
};

template <typename color_tag>
struct masker<active_tag, color_tag> {
  static constexpr board_t mask(const node_t& node) noexcept {
    typedef typename color_traits<color_tag>::other other_tag;
    return node.occupy<other_tag>();
  }
};

template <typename piece_tag, typename color_tag>
struct generator {
  template <typename moves_tag>
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept {
    auto const mask = masker<moves_tag, color_tag>::mask(node);
    for (const auto from : bsf(node.occupy<piece_tag, color_tag>()))
      for (const auto to : bsf(attacker<piece_tag, color_tag>::attack(
                                   board_of(from), node.occupy()) &
                               mask))
        *moves++ = move_t{from, to};
    return moves;
  }
};

template <typename color_tag>
struct generator<knight_tag, color_tag> {
  template <typename moves_tag>
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept {
    auto const mask = masker<moves_tag, color_tag>::mask(node);
    for (const auto from : bsf(node.occupy<knight_tag, color_tag>()))
      for (const auto to :
           bsf(attacker<knight_tag, color_tag>::attack(board_of(from)) & mask))
        *moves++ = move_t{from, to};
    return moves;
  }
};

template <typename color_tag>
struct generator<king_tag, color_tag> {
  template <typename moves_tag>
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept {
    typedef typename color_traits<color_tag>::other other_tag;
    auto const mask =
        masker<moves_tag, color_tag>::mask(node) & ~node.attack<other_tag>();
    const square_t from = node.king<color_tag>();
    for (const auto to :
         bsf(attacker<king_tag, color_tag>::attack(board_of(from)) & mask))
      *moves++ = move_t{from, to};
    return castle<moves_tag>(node, moves);
  }

 private:
  template <typename moves_tag>
  static moves_t::iterator castle(const node_t& node,
                                  moves_t::iterator moves) noexcept {
    return moves;
  }
};

template <>
template <>
inline moves_t::iterator generator<king_tag, white_tag>::castle<all_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  if ((node.castle() & white_castle_king) && !(node.occupy() & (F1 | G1)) &&
      !(node.attack<black_tag>() & (E1 | F1 | G1)))
    *moves++ = move_t{e1, g1};
  if ((node.castle() & white_castle_queen) &&
      !(node.occupy() & (B1 | C1 | D1)) &&
      !(node.attack<black_tag>() & (C1 | D1 | E1)))
    *moves++ = move_t{e1, c1};
  return moves;
}

template <>
template <>
inline moves_t::iterator generator<king_tag, black_tag>::castle<all_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  if ((node.castle() & black_castle_king) && !(node.occupy() & (F8 | G8)) &&
      !(node.attack<white_tag>() & (E8 | F8 | G8)))
    *moves++ = move_t{e8, g8};
  if ((node.castle() & black_castle_queen) &&
      !(node.occupy() & (B8 | C8 | D8)) &&
      !(node.attack<white_tag>() & (C8 | D8 | E8)))
    *moves++ = move_t{e8, c8};
  return moves;
}

template <typename color_tag>
struct generator<pawn_tag, color_tag> {
  template <typename moves_tag>
  static moves_t::iterator generate(const node_t& node,
                                    moves_t::iterator moves) noexcept;

 private:
  static moves_t::iterator generate_moves(const board_t board,
                                          const square_t delta,
                                          moves_t::iterator moves) noexcept;

  static moves_t::iterator generate_promotions(
      const square_t from, const square_t to,
      moves_t::iterator moves) noexcept {
    for (std::uint8_t promotion = move_t::Q; promotion > move_t::no; --promotion)
      *moves++ = move_t{from, to, move_t::promotion_t(promotion)};
    return moves;
  }
};

template <>
inline moves_t::iterator generator<pawn_tag, white_tag>::generate_moves(
    const board_t board, const square_t delta,
    moves_t::iterator moves) noexcept {
  for (const auto to : bsf(board)) {
    const square_t from(to - delta);
    if (to < a8)
      *moves++ = move_t{from, to};
    else
      moves = generate_promotions(from, to, moves);
  }
  return moves;
}

template <>
inline moves_t::iterator generator<pawn_tag, black_tag>::generate_moves(
    const board_t board, const square_t delta,
    moves_t::iterator moves) noexcept {
  for (const auto to : bsf(board)) {
    const square_t from(to + delta);
    if (to > h1)
      *moves++ = move_t{from, to};
    else
      moves = generate_promotions(from, to, moves);
  }
  return moves;
}

template <>
template <>
inline moves_t::iterator generator<pawn_tag, white_tag>::generate<active_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  constexpr __v4du mask{~Fa, ~Fh, R7};
  constexpr __v4du shift{7, 9, 8};
  const board_t empty = ~node.occupy();
  const board_t attack =
      node.occupy<black_tag>() | (board_of(node.en_passant()) & R6);
  const __v4du mask2{attack, attack, empty};
#ifdef __clang__
  const __v4du board =
      ((board4_of(node.occupy<pawn_tag, white_tag>()) & mask) << shift) & mask2;
#else
  const __v4du board =
      ((node.occupy<pawn_tag, white_tag>() & mask) << shift) & mask2;
#endif
  moves = generate_moves(board[0], 7, moves);
  moves = generate_moves(board[1], 9, moves);
  moves = generate_moves(board[2], 8, moves);
  return moves;
}

template <>
template <>
inline moves_t::iterator generator<pawn_tag, white_tag>::generate<all_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  constexpr __v4du mask{~Fa, ~Fh, ~0UL, R2};
  constexpr __v4du shift{7, 9, 8, 8};
  const board_t empty = ~node.occupy();
  const board_t attack =
      node.occupy<black_tag>() | (board_of(node.en_passant()) & R6);
  const __v4du mask2{attack, attack, empty, empty};
#ifdef __clang__
  const __v4du board =
      ((board4_of(node.occupy<pawn_tag, white_tag>()) & mask) << shift) & mask2;
#else
  const __v4du board =
      ((node.occupy<pawn_tag, white_tag>() & mask) << shift) & mask2;
#endif
  moves = generate_moves(board[0], 7, moves);
  moves = generate_moves(board[1], 9, moves);
  moves = generate_moves(board[2], 8, moves);
  moves = generate_moves((board[3] << 8) & empty, 16, moves);
  return moves;
}

template <>
template <>
inline moves_t::iterator generator<pawn_tag, black_tag>::generate<active_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  constexpr __v4du mask{~Fh, ~Fa, R2};
  constexpr __v4du shift{7, 9, 8};
  const board_t empty = ~node.occupy();
  const board_t attack =
      node.occupy<white_tag>() | (board_of(node.en_passant()) & R3);
  const __v4du mask2{attack, attack, empty};
#ifdef __clang__
  const __v4du board =
      ((board4_of(node.occupy<pawn_tag, black_tag>()) & mask) >> shift) & mask2;
#else
  const __v4du board =
      ((node.occupy<pawn_tag, black_tag>() & mask) >> shift) & mask2;
#endif
  moves = generate_moves(board[0], 7, moves);
  moves = generate_moves(board[1], 9, moves);
  moves = generate_moves(board[2], 8, moves);
  return moves;
}

template <>
template <>
inline moves_t::iterator generator<pawn_tag, black_tag>::generate<all_tag>(
    const node_t& node, moves_t::iterator moves) noexcept {
  constexpr __v4du mask{~Fh, ~Fa, ~0UL, R7};
  constexpr __v4du shift{7, 9, 8, 8};
  const board_t empty = ~node.occupy();
  const board_t attack =
      node.occupy<white_tag>() | (board_of(node.en_passant()) & R3);
  const __v4du mask2{attack, attack, empty, empty};
#ifdef __clang__
  const __v4du board =
      ((board4_of(node.occupy<pawn_tag, black_tag>()) & mask) >> shift) & mask2;
#else
  const __v4du board =
      ((node.occupy<pawn_tag, black_tag>() & mask) >> shift) & mask2;
#endif
  moves = generate_moves(board[0], 7, moves);
  moves = generate_moves(board[1], 9, moves);
  moves = generate_moves(board[2], 8, moves);
  moves = generate_moves((board[3] >> 8) & empty, 16, moves);
  return moves;
}

}  // namespace detail

template <typename moves_tag>
move_generator<moves_tag>::move_generator(
    const node_t& node, const history_table_t& h_table,
    const history_table_t& b_table) noexcept
    : _end(generate(node, _moves.begin())) {
  for (std::uint8_t index = 0; index < size(); ++index) {
    move_t& move = _moves[index];
    eval_t& eval = _evals[index];
    move.metadata.index = index;
    eval.score = exchanger::evaluate(node, move);
    eval.good = static_cast<float>(h_table(node, move)) /
                static_cast<float>(b_table(node, move));
  }

  const auto good =
      std::partition(_moves.begin(), _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score > 0_P;
      });
  const auto zero =
      std::partition(good, _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score == 0_P;
      });
  // end = std::partition(zero, _end, [this](const move_t& move) noexcept
  // {return _evals[move.index].score >= -2_P;});

  const auto cmp_score_history = [this](const move_t& move1,
                                        const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    if (eval1.score != eval2.score) return eval1.score > eval2.score;
    return eval1.good > eval2.good;
  };

  const auto cmp_history = [this](const move_t& move1,
                                  const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    return eval1.good > eval2.good;
  };

  std::sort(_moves.begin(), good, cmp_score_history);
  std::sort(good, zero, cmp_history);
  std::sort(zero, _end, cmp_score_history);
}

template <typename moves_tag>
move_generator<moves_tag>::move_generator(
    const node_t& node, const history_table_t& h_table,
    const history_table_t& b_table, const killer_table_t::entry_t& k_entry1,
    const killer_table_t::entry_t& k_entry2) noexcept
    : _end(generate(node, _moves.begin())) {
  for (std::uint8_t index = 0; index < size(); ++index) {
    move_t& move = _moves[index];
    eval_t& eval = _evals[index];
    move.metadata.index = index;
    eval.score = exchanger::evaluate(node, move);
    eval.good = static_cast<float>(h_table(node, move)) /
                static_cast<float>(b_table(node, move));
    eval.killer = 4 * (k_entry1[0] == move || k_entry1[1] == move) +
                  (k_entry2[0] == move || k_entry2[1] == move);
  }

  // begin good killer zero end
  const auto good =
      std::partition(_moves.begin(), _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score > 0;
      });
  const auto zero =
      std::partition(good, _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score == 0;
      });
  const auto killer =
      std::partition(good, zero, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].killer > 0;
      });

  const auto cmp_score_history = [this](const move_t& move1,
                                        const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    if (eval1.score != eval2.score) return eval1.score > eval2.score;
    return eval1.good > eval2.good;
  };

  const auto cmp_killer_history = [this](const move_t& move1,
                                         const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    if (eval1.killer != eval2.killer) return eval1.killer > eval2.killer;
    return eval1.good > eval2.good;
  };

  const auto cmp_history = [this](const move_t& move1,
                                  const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    return eval1.good > eval2.good;
  };

  std::sort(_moves.begin(), good, cmp_score_history);
  std::sort(good, killer, cmp_killer_history);
  std::sort(killer, zero, cmp_history);
  std::sort(zero, _end, cmp_score_history);
}

template <typename moves_tag>
move_generator<moves_tag>::move_generator(
    const node_t& node, const history_table_t& h_table,
    const history_table_t& b_table, const killer_table_t::entry_t& k_entry1,
    const killer_table_t::entry_t& k_entry2,
    const killer_table_t::entry_t& k_entry3) noexcept
    : _end(generate(node, _moves.begin())) {
  for (std::uint8_t index = 0; index < size(); ++index) {
    move_t& move = _moves[index];
    eval_t& eval = _evals[index];
    move.metadata.index = index;
    eval.score = exchanger::evaluate(node, move);
    eval.good = static_cast<float>(h_table(node, move)) /
                static_cast<float>(b_table(node, move));
    eval.killer = 4 * (k_entry1[0] == move || k_entry1[1] == move) +
                  2 * (k_entry2[0] == move || k_entry2[1] == move) +
                  (k_entry3[0] == move || k_entry3[1] == move);
  }

  // begin good killer zero end
  const auto good =
      std::partition(_moves.begin(), _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score > 0;
      });
  const auto zero =
      std::partition(good, _end, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].score == 0;
      });
  const auto killer =
      std::partition(good, zero, [this](const move_t& move) noexcept {
        return _evals[move.metadata.index].killer > 0;
      });

  const auto cmp_score_history = [this](const move_t& move1,
                                        const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    if (eval1.score != eval2.score) return eval1.score > eval2.score;
    return eval1.good > eval2.good;
  };

  const auto cmp_killer_history = [this](const move_t& move1,
                                         const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    if (eval1.killer != eval2.killer) return eval1.killer > eval2.killer;
    return eval1.good > eval2.good;
  };

  const auto cmp_history = [this](const move_t& move1,
                                  const move_t& move2) noexcept {
    const eval_t& eval1 = _evals[move1.metadata.index];
    const eval_t& eval2 = _evals[move2.metadata.index];
    return eval1.good > eval2.good;
  };

  std::sort(_moves.begin(), good, cmp_score_history);
  std::sort(good, killer, cmp_killer_history);
  std::sort(killer, zero, cmp_history);
  std::sort(zero, _end, cmp_score_history);
}

// template <typename moves_tag>
// move_generator<moves_tag>::move_generator(const node_t& node, const
// history_table_t& h_table, const history_table_t& b_table, const
// killer_table_t::entry_t& k_entry1, const killer_table_t::entry_t& k_entry2)
// noexcept
//:
//	_end(generate(node, _moves.begin()))
//{
//	for (std::uint8_t index = 0; index < size(); ++index)
//	{
//		move_t& move = _moves[index];
//		eval_t& eval = _evals[index];
//		move.index = index;
//		eval.score = exchanger::evaluate(node, move);
//		eval.good = float(h_table(node, move)) / float(b_table(node,
// move));
//		eval.killer = (k_entry1[0] == move || k_entry1[1] == move) +
//(k_entry2[0] == move || k_entry2[1] == move);
//	}
//
//	std::stable_sort
//	(
//		_moves.begin(), _end,
//		[this](const move_t& move1, const move_t& move2) noexcept
//		{
//			const eval_t& eval1 = _evals[move1.index];
//			const eval_t& eval2 = _evals[move2.index];
//			if (eval1.score != eval2.score)
//				return eval1.score > eval2.score;
//			if (eval1.killer != eval2.killer)
//				return eval1.killer > eval2.killer;
//			return eval1.good > eval2.good;
//		}
//	);
//}

template <typename moves_tag>
moves_t::iterator move_generator<moves_tag>::generate(
    const node_t& node, moves_t::iterator moves) noexcept {
  return node.color() == white ? generate<white_tag>(node, moves)
                               : generate<black_tag>(node, moves);
}

template <typename moves_tag>
template <typename color_tag>
moves_t::iterator move_generator<moves_tag>::generate(
    const node_t& node, moves_t::iterator moves) noexcept {
  moves = detail::generator<king_tag, color_tag>::template generate<moves_tag>(
      node, moves);
  moves =
      detail::generator<rook_queen_tag,
                        color_tag>::template generate<moves_tag>(node, moves);
  moves =
      detail::generator<bishop_queen_tag,
                        color_tag>::template generate<moves_tag>(node, moves);
  moves =
      detail::generator<knight_tag, color_tag>::template generate<moves_tag>(
          node, moves);
  moves = detail::generator<pawn_tag, color_tag>::template generate<moves_tag>(
      node, moves);
  return moves;
}

template class move_generator<all_tag>;
template class move_generator<active_tag>;

}  // namespace chess
