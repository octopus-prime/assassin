/*
 * searcher.cpp
 *
 *  Created on: 20.08.2016
 *      Author: mike
 */

#include <tbb/task_group.h>
#include <boost/smart_ptr/detail/spinlock.hpp>
#include <mutex>
#include <sstream>
#include "attacker.hpp"
#include "evaluator.hpp"
#include "io.hpp"
#include "move_generator.hpp"
#include "searcher.hpp"

namespace chess {

searcher::searcher(transposition_table_t& t_table,
                   const report_t& report) noexcept
    : _report(report),
      _t_table(t_table),
      _h_table(),
      _b_table(),
      _k_table(),
      _count(),
      _height(),
      _stop(),
      _start() {}

score_t searcher::operator()(const node_t& node,
                             const std::uint_fast8_t depth) {
  score_t score = search(node, -30000, +30000);
  //	score_t score = evaluator::evaluate(node);;
  _start = clock::now();

  for (std::uint_fast8_t iteration = 1; iteration <= depth; ++iteration) {
    //		score = search(node, -30000, +30000, iteration * PLY, 0, move_t
    //{});

    int alpha = score - 25;
    int beta = score + 25;

    for (score_t delta = 50; delta < 30000; delta *= 2) {
      score = search(node, alpha, beta, iteration * PLY, 0, move_t{});

      if (_stop) break;

      _report(iteration, _height, score * node.color(), clock::now() - _start,
              _count, get_pv(node, iteration));

      if (-30000 + 100 > score || score > +30000 - 100) break;

      if (score <= alpha)
        alpha -= delta;
      else if (score >= beta)
        beta += delta;
      else
        break;

      alpha = std::max(alpha, -30000);
      beta = std::min(beta, +30000);
    }

    if (_stop) break;

    if (-30000 + 100 > score || score > +30000 - 100) break;
    /*

                    score = search(node, alpha, beta, iteration * PLY, 0, move_t
       {});

                    if (_stop)
                            break;

                    if (score <= alpha)
                    {
                            _report(iteration, _height, score * node.color(),
       clock::now() - _start, _count, get_pv(node, iteration)); score =
       search(node, -30000, score, iteration * PLY, 0, move_t {});
                    }
                    else if (score >= beta)
                    {
                            _report(iteration, _height, score * node.color(),
       clock::now() - _start, _count, get_pv(node, iteration)); score =
       search(node, score, +30000, iteration * PLY, 0, move_t {});
                    }

                    if (_stop)
                            break;

                    _report(iteration, _height, score * node.color(),
       clock::now() - _start, _count, get_pv(node, iteration));

                    if (-30000 + 100 > score || score > +30000 - 100)
                            break;
    */
  }

  return score;
}

void searcher::stop() { _stop = true; }

const score_t razor_margin[4] = {483, 570, 603, 554};

constexpr score_t futility_margin(std::uint_fast8_t d) noexcept {
  return 150 * d;
}

score_t searcher::search(const node_t& node, const score_t alpha,
                         const score_t beta, std::uint_fast8_t depth,
                         const std::uint_fast8_t height, const move_t moved) {
  if (_stop) return 0;

  if (height > _height) _height = height;

//  if (count_repetition(node) >= 3 || node.half_moves() >= 50)
  if (node.is_3fold_repetition() || node.is_50_moves())
    return 0;

  const bool check = test_check(node, node.color());
  depth += check * PLY;

  if (node.color() == white) {
    if (moved != move_t{} && node[moved.to] == p) {
      switch (rank_of(moved.to)) {
        case 2:
          depth += PLY / 2;
          break;
        case 1:
          depth += PLY / 1;
          break;
      }
    }
  } else {
    if (moved != move_t{} && node[moved.to] == P) {
      switch (rank_of(moved.to)) {
        case 5:
          depth += PLY / 2;
          break;
        case 6:
          depth += PLY / 1;
          break;
      }
    }
  }

  ++_count;

  move_t best_move = {};
  score_t search_alpha = alpha;
  score_t search_beta = beta;

  const transposition_table_t::entry_t* const entry = _t_table[node];
  if (entry) {
    if (entry->depth >= depth) {
      const score_t score = entry->score;
      switch (entry->flag) {
        case transposition_table_t::EXACT:
          return score;
        case transposition_table_t::LOWER:
          if (score > search_alpha) search_alpha = score;
          break;
        case transposition_table_t::UPPER:
          if (score < search_beta) search_beta = score;
          break;
        default:
          break;
      }
      if (search_alpha >= search_beta) return search_alpha;
    }
    best_move = entry->move;
  }

  if (depth < PLY) {
    --_count;
    //		if (height > 10)
    //			return evaluator::evaluate(node);;
    return search(node, search_alpha, search_beta);
  }

  const score_t eval = node.score() * node.color();

  if (!check && depth < 4 && eval + razor_margin[depth] <= alpha &&
      best_move == move_t{}) {
    const score_t ralpha = alpha - razor_margin[depth];
    const score_t score = search(node, ralpha, ralpha + 1);
    if (score <= ralpha) return score;
  }

  if (!check && depth < 7 && eval - futility_margin(depth) >= beta &&
      eval < 20000)  // VALUE_KNOWN_WIN &&
                     // pos.non_pawn_material(pos.side_to_move()))
  {
    return eval - futility_margin(depth);
  }

  const std::uint_fast8_t reduction = (3 + (depth > 6)) * PLY;
  const bool skip = entry && entry->depth >= depth - reduction &&
                    entry->score < search_beta &&
                    entry->flag == transposition_table_t::UPPER;
  if (depth >= reduction && moved != move_t{} && !check && try_null(node) &&
      !skip) {
    const square_t en_passant = const_cast<node_t&>(node).flip(0);
    const score_t score = -search(node, -search_beta, -search_beta + 1,
                                  depth - reduction, height + 1, move_t());
    const_cast<node_t&>(node).flip(en_passant);
    if (score >= search_beta) return score;
  }

  if (best_move == move_t{} && depth > 2 * PLY) {
    search(node, search_alpha, search_beta, depth - 2 * PLY, height + 1,
           best_move);
    const transposition_table_t::entry_t* const entry = _t_table[node];
    if (entry) best_move = entry->move;
  }

  std::atomic_int_fast32_t legal(0);
  score_t best_score = search_alpha;

  if (best_move != move_t{}) {
    try {
      const node_t succ(node, best_move);
      if (!test_check(succ, node.color())) {
        //				_t_table.prefetch(succ);
        legal++;
        _b_table.put(node, best_move);
        std::uint_fast8_t ext = (moved.to == best_move.to) * PLY / 4;
        const score_t score =
            -search(succ, -search_beta, -best_score, depth - 1 * PLY + ext,
                    height + 1, best_move);
        if (score > best_score) {
          if (score >= search_beta) {
            _t_table.put(node, best_move, search_beta,
                         transposition_table_t::LOWER, depth + ext);
            _h_table.put(node, best_move);
            _k_table.put(node, best_move, height);
            return score;
          }
          best_score = score;
        }
      }
    } catch (...) {
      // Illegal move detected. Could come from type-1 collision in TT. Discard
      // "best" move.
      best_move = move_t{};
    }
  }

  bool prune = false;
  score_t scoreX = 0;
  const score_t scoreM = node.score() * node.color();
  switch (depth / PLY) {
    case 3:
      if (!check && scoreM + 1_Q <= best_score) depth -= PLY;
      break;
    case 2:
      scoreX = scoreM + 1_R;
      if (scoreX <= best_score) prune = true;
      break;
    case 1:
      scoreX = scoreM + 1_N;
      if (scoreX <= best_score) prune = true;
      break;
  }

  constexpr killer_table_t::entry_t k_entry{};
  const move_generator<all_tag> moves(
      node, _h_table, _b_table, _k_table[height], _k_table[height + 2],
      height > 2 ? _k_table[height - 2] : k_entry);

  if (!check && depth > 3 * PLY && moves.size() > 8) {
    boost::detail::spinlock mutex = BOOST_DETAIL_SPINLOCK_INIT;
    tbb::task_group group;

    const auto work = [&](const node_t& succ, const move_t move) {
//      std::uint_fast8_t ext = (moved.to == move.to) * PLY / 4;
        std::uint_fast8_t ext = (moved.to == move.to) * PLY / 4 + (moves.score(move) > 0) * PLY / 2 + (moves.score(move) < 0) * PLY / 4;

      score_t score;
      if (group.is_canceling()) return;
      score = -search(succ, -best_score - 1, -best_score, depth - 1 * PLY + ext,
                      height + 1, move);
      if (group.is_canceling()) return;
      if (score > best_score && score < search_beta) {
        //				auto a = score + 25;
        //				score = -search(succ, -a, -score, depth
        //-
        // 1
        //*  PLY + ext, height + 1, move); 			if
        //(group.is_canceling()) 				return;
        //			if (score > best_score && score == a)
        score = -search(succ, -search_beta, -score, depth - 1 * PLY + ext,
                        height + 1, move);
        //			if (group.is_canceling())
        //				return;
      }
      std::unique_lock<boost::detail::spinlock> lock(mutex);
      if (score > best_score) {
        best_score = score;
        best_move = move;
        if (score >= search_beta) {
          group.cancel();
          _t_table.put(node, move, search_beta, transposition_table_t::LOWER,
                       depth + ext);
          _h_table.put(node, move);
          _k_table.put(node, move, height);
        }
      }
    };

    const auto work2 = [&](const node_t& succ, const move_t move,
                           const std::uint8_t reduction) {
      const score_t score =
          -search(succ, -best_score - 1, -best_score,
                  depth - 2 * PLY - reduction, height + 1, move);
      if (score <= best_score) return;
      if (group.is_canceling()) return;
      work(succ, move);
    };

    for (auto move : moves) {
      if (group.is_canceling()) break;
      if (move == best_move) continue;
      const node_t succ(node, move);
      if (test_check(succ, node.color())) continue;
      //			_t_table.prefetch(succ);
      legal++;
      _b_table.put(node, move);

      const score_t mscore = moves.score(move);
      const bool killer = moves.killer(move);
      move.metadata.check = test_check(succ, succ.color());
      if (legal && !check && !move.metadata.check && prune &&
          mscore + scoreX <= best_score && !killer)
        continue;

      if (group.is_canceling()) break;
      if (legal > 4 && depth > 3 * PLY && node[move.to] == no_piece &&
          node[move.promotion] == 0 && !check && !move.metadata.check &&
          !killer)
        //			if (legal > 4 && depth > 3 && mscore <= 0 &&
        //! check
        //&& !move.index)
        group.run(std::bind(work2, succ, move, (mscore < -1_P) * PLY / 2));
      else  // if (legal > 2)
        group.run(std::bind(work, succ, move));
      //			else
      //				work(succ, move);
    }

    group.wait();

    if (best_score >= search_beta) return best_score;
  } else {
    for (auto move : moves) {
      if (move == best_move) continue;
      const node_t succ(node, move);
      if (test_check(succ, node.color())) continue;
      //			_t_table.prefetch(succ);
      legal++;
      _b_table.put(node, move);

      const score_t mscore = moves.score(move);
      const bool killer = moves.killer(move);
      move.metadata.check = test_check(succ, succ.color());
      if (legal && !check && !move.metadata.check && prune &&
          mscore + scoreX <= best_score && !killer)
        continue;

      if (legal > 4 && depth > 3 * PLY && node[move.to] == no_piece &&
          node[move.promotion] == 0 && !check && !move.metadata.check &&
          !killer)
      //			if (legal > 4 && depth > 3 && mscore <= 0 &&
      //! check && !move.index)
      {
        const score_t score = -search(
            succ, -best_score - 1, -best_score,
            depth - 2 * PLY - (mscore < -1_P) * PLY / 2, height + 1, move);
        if (score <= best_score) continue;
      }

      std::uint_fast8_t ext = (moved.to == move.to) * PLY / 4 + (moves.score(move) > 0) * PLY / 2 + (moves.score(move) < 0) * PLY / 4;

      score_t score;
      score = -search(succ, -best_score - 1, -best_score, depth - 1 * PLY + ext,
                      height + 1, move);
      if (score > best_score && score < search_beta) {
        //				auto a = score + 25;
        //				score = -search(succ, -a, -score, depth
        //-
        // 1
        //*  PLY + ext, height + 1, move); 			if (score >
        // best_score && score == a)
        score = -search(succ, -search_beta, -score, depth - 1 * PLY + ext,
                        height + 1, move);
      }
      if (score > best_score) {
        if (score >= search_beta) {
          _t_table.put(node, move, search_beta, transposition_table_t::LOWER,
                       depth + ext);
          _h_table.put(node, move);
          _k_table.put(node, move, height);
          return score;
        }
        best_score = score;
        best_move = move;
      }
    }
  }

  if (!legal) {
    const score_t score = check ? -30000 + height : 0;
    _t_table.put(node, move_t{}, score, transposition_table_t::EXACT, depth);
    return score;
  }

  if (best_score > alpha) {
    _t_table.put(node, best_move, best_score, transposition_table_t::EXACT,
                 depth);
    _h_table.put(node, best_move);
  } else
    _t_table.put(node, best_move, best_score, transposition_table_t::UPPER,
                 depth);

  return best_score;
}

// score_t
// foo(const node_t& node) noexcept
//{
//	return node.color() == white
//			? 1_Q + bool(node.occupy<pawn_tag, white_tag>() & R7) *
//(1_Q
//- 1_P)
//			: 1_Q + bool(node.occupy<pawn_tag, black_tag>() & R2) *
//(1_Q
//- 1_P);
//}

score_t searcher::search(const node_t& node, const score_t alpha,
                         const score_t beta) {
  if (_stop) return 0;

  _count++;

  score_t best_score = alpha;

  const score_t score = evaluator::evaluate(node);

  if (node.is_quiet())
	  return score;

  if (score > best_score) {
    if (score >= beta) return score;
    best_score = score;
  }

  //	const bool check = test_check(node, node.color());

  //	if (/*!check && */score + foo(node) < alpha)
  //		return best_score;

  const move_generator<active_tag> moves(node, _h_table, _b_table);

  for (const auto move : moves) {
    //		if (!check && moves.score(move) < -2_P)

    //		if (node.score() * node.color() + moves.score(move) + 2_P <
    // best_score) 			break;
    const node_t succ(node, move);
    if (test_check(succ, node.color())) continue;
    const score_t score = -search(succ, -beta, -best_score);
    if (score > best_score) {
      if (score >= beta) return score;
      best_score = score;
    }
  }

  return best_score;
}

constexpr bool searcher::test_check(const node_t& node,
                                    const color_t color) noexcept {
  return color == white
             ? node.occupy<king_tag, white_tag>() & node.attack<black_tag>()
             : node.occupy<king_tag, black_tag>() & node.attack<white_tag>();
}

//inline std::uint_fast8_t
//searcher::count_repetition(const node_t& leaf) noexcept {
//  std::uint_fast8_t count = 1;
//  for (const node_t* node = leaf.parent(); node; node = node->parent()) {
//    count += (leaf.hash() == node->hash());
//    if (!node->half_moves())
//      break;
//  }
//  return count;
//}

template <typename color_tag>
inline bool searcher::try_null(const node_t& node) noexcept {
  typedef typename color_traits<color_tag>::other other_tag;
  return popcnt(node.occupy<color_tag>()) > 3 &&
         (node.occupy<knight_tag, color_tag>() |
          node.occupy<bishop_queen_tag, color_tag>() |
          node.occupy<rook_queen_tag, color_tag>()) &&
         !(node.attack<other_tag>() &
           detail::attacker<king_tag, color_tag>::attack(
               node.occupy<king_tag, color_tag>()));
}

inline bool searcher::try_null(const node_t& node) noexcept {
  return (node.color() == white) ? try_null<white_tag>(node)
                                 : try_null<black_tag>(node);
}

std::string searcher::get_pv(node_t node,
                             const std::uint_fast8_t length) const {
  std::ostringstream stream;
  for (std::uint_fast8_t i = 0; i < length; ++i) {
    const transposition_table_t::entry_t* const entry = _t_table[node];
    if (!entry) break;
    const move_t move = entry->move;
    if (move == move_t{}) break;
    stream << std::make_pair(std::cref(node), std::cref(move)) << ' ';
    node = node_t(node, move);
  }
  return std::move(stream.str());
}

}  // namespace chess
