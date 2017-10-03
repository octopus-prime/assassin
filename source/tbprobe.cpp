/*
  Copyright (c) 2013-2015 Ronald de Man
  This file may be redistributed and/or modified without restrictions.

  tbprobe.cpp contains the Stockfish-specific routines of the
  tablebase probing code. It should be relatively easy to adapt
  this code to other chess engines.
*/

// The probing code currently expects a little-endian architecture (e.g. x86).

// Define DECOMP64 when compiling for a 64-bit platform.
// 32-bit is only supported for 5-piece tables, because tables are mmap()ed
// into memory.
#define IS_64BIT
#ifdef IS_64BIT
#define DECOMP64
#endif

#include "node.hpp"
#include "io.hpp"
//#include "movegen.h"
//#include "bitboard.h"
//#include "search.h"
//#include "bitcount.h"

#include "tbprobe.h"
#include "tbcore.h"

//namespace Zobrist {
//  extern Key psq[COLOR_NB][PIECE_TYPE_NB][SQUARE_NB];
//}

namespace chess {

int TBlargest = 0;

#include "tbcore.c"

typedef const node_t Position;

template <typename Position>
struct Adapter
{
	constexpr
	Adapter(const Position& position) noexcept
	:
		_position(position)
	{
	}

	void
	build_str(char* str, const int mirror) const noexcept;

	uint64
	calc_key(const int mirror) const noexcept;

	bool
	is_white() const noexcept;

private:
	const Position& _position;
};

template <typename color_tag>
char* prt_str_2(const Position& pos, char* str)
{
	str = std::fill_n(str, popcnt(pos.occupy<queen_tag, color_tag>()), 'Q');
	str = std::fill_n(str, popcnt(pos.occupy<rook_tag, color_tag>()), 'R');
	str = std::fill_n(str, popcnt(pos.occupy<bishop_tag, color_tag>()), 'B');
	str = std::fill_n(str, popcnt(pos.occupy<knight_tag, color_tag>()), 'N');
	str = std::fill_n(str, popcnt(pos.occupy<pawn_tag, color_tag>()), 'P');
	return str;
}

template <typename color_tag>
void prt_str_1(const Position& pos, char *str)
{
	typedef typename color_traits<color_tag>::other other_tag;
	*str++ = 'K';
	str = prt_str_2<color_tag>(pos, str);
	*str++ = 'v';
	*str++ = 'K';
	str = prt_str_2<other_tag>(pos, str);
	*str++ = 0;
}

template <>
inline void
Adapter<node_t>::build_str(char* str, const int mirror) const noexcept
{
	if (mirror)
		prt_str_1<black_tag>(_position, str);
	else
		prt_str_1<white_tag>(_position, str);
}

template<>
inline uint64
Adapter<node_t>::calc_key(const int mirror) const noexcept
{
}

template<>
constexpr bool
Adapter<node_t>::is_white() const noexcept
{
	return _position.color() == white;
}

// Given a position with 6 or fewer pieces, produce a text string
// of the form KQPvKRP, where "KQP" represents the white pieces if
// mirror == 0 and the black pieces if mirror == 1.
// No need to make this very efficient.
static void prt_str(const Position& pos, char *str, int mirror)
{
	Adapter<node_t> adapter(pos);
	return adapter.build_str(str, mirror);
//	char* x = str;
//	if (mirror)
//		prt_str_1<black_tag>(pos, str);
//	else
//		prt_str_1<white_tag>(pos, str);
//	std::cout << x << std::endl;

//  color color;
//  piece_t pt;
//  int i;
//
//  color = !mirror ? white : black;
//  for (pt = KING; pt >= PAWN; pt--)
//    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
//      *str++ = pchr[6 - pt];
//  *str++ = 'v';
//  color = ~color;
//  for (pt = KING; pt >= PAWN; pt--)
//    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
//      *str++ = pchr[6 - pt];
//  *str++ = 0;
}

// Given a position, produce a 64-bit material signature key.
// If the engine supports such a key, it should equal the engine's key.
// Again no need to make this very efficient.
static uint64 calc_key(Position& pos, int mirror)
{
//  Color color;
//  PieceType pt;
//  int i;
  uint64 key = 0;

  for (size_t i = 0; i < popcnt(pos.occupy<pawn_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[1][i];
  for (size_t i = 0; i < popcnt(pos.occupy<knight_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[2][i];
  for (size_t i = 0; i < popcnt(pos.occupy<bishop_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[3][i];
  for (size_t i = 0; i < popcnt(pos.occupy<rook_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[4][i];
  for (size_t i = 0; i < popcnt(pos.occupy<queen_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[5][i];
  for (size_t i = 0; i < popcnt(pos.occupy<king_tag, white_tag>()); ++i)
	  key ^= piece_square_hash[6][i];
  for (size_t i = 0; i < popcnt(pos.occupy<pawn_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[7][i];
  for (size_t i = 0; i < popcnt(pos.occupy<knight_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[8][i];
  for (size_t i = 0; i < popcnt(pos.occupy<bishop_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[9][i];
  for (size_t i = 0; i < popcnt(pos.occupy<rook_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[10][i];
  for (size_t i = 0; i < popcnt(pos.occupy<queen_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[11][i];
  for (size_t i = 0; i < popcnt(pos.occupy<king_tag, black_tag>()); ++i)
	  key ^= piece_square_hash[12][i];
//  key ^= piece_square_hash[2][pos.occupy<knight_tag, white_tag>()];
//  key ^= piece_square_hash[3][pos.occupy<bishop_tag, white_tag>()];
//  key ^= piece_square_hash[4][pos.occupy<rook_tag, white_tag>()];
//  key ^= piece_square_hash[5][pos.occupy<queen_tag, white_tag>()];
//  key ^= piece_square_hash[6][pos.occupy<king_tag, white_tag>()];
//  key ^= piece_square_hash[7][pos.occupy<pawn_tag, black_tag>()];
//  key ^= piece_square_hash[8][pos.occupy<knight_tag, black_tag>()];
//  key ^= piece_square_hash[9][pos.occupy<bishop_tag, black_tag>()];
//  key ^= piece_square_hash[10][pos.occupy<rook_tag, black_tag>()];
//  key ^= piece_square_hash[11][pos.occupy<queen_tag, black_tag>()];
//  key ^= piece_square_hash[12][pos.occupy<king_tag, black_tag>()];

//  key |= popcnt(pos.occupy<queen_tag>()) << 0;
//  key |= popcnt(pos.occupy<rook_tag>()) << 8;
//  key |= popcnt(pos.occupy<bishop_tag>()) << 16;
//  key |= popcnt(pos.occupy<knight_tag>()) << 24;
//  key |= popcnt(pos.occupy<pawn_tag>()) << 32;

  if (mirror)
	  key ^= color_hash;
//
//  color = !mirror ? WHITE : BLACK;
//  for (pt = PAWN; pt <= KING; pt++)
//    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
//      key ^= Zobrist::psq[WHITE][pt][i - 1];
//  color = ~color;
//  for (pt = PAWN; pt <= KING; pt++)
//    for (i = popcount<Max15>(pos.pieces(color, pt)); i > 0; i--)
//      key ^= Zobrist::psq[BLACK][pt][i - 1];

  return key;
}

// Produce a 64-bit material key corresponding to the material combination
// defined by pcs[16], where pcs[1], ..., pcs[6] is the number of white
// pawns, ..., kings and pcs[9], ..., pcs[14] is the number of black
// pawns, ..., kings.
// Again no need to be efficient here.
static uint64 calc_key_from_pcs(int *pcs, int mirror)
{
//  int color;
//  PieceType pt;
//  int i;
  uint64 key = 0;

  for (int i = 0; i < 6; ++i)
	  for (int j = 0; j < pcs[i]; ++j)
		  key ^= piece_square_hash[i+1][j];

  for (int i = 8; i < 14; ++i)
	  for (int j = 0; j < pcs[i]; ++j)
		  key ^= piece_square_hash[i-1][j];

//  int s = 0;
//  for (int i = 0; i < 6; ++i, s += 4)
//	  key |= pcs[i] << s;
//  for (int i = 8; i < 14; ++i, s += 4)
//	  key |= pcs[i] << s;

  if (mirror)
	  key ^= chess::color_hash;
//
//  color = !mirror ? 0 : 8;
//  for (pt = PAWN; pt <= KING; pt++)
//    for (i = 0; i < pcs[color + pt]; i++)
//      key ^= Zobrist::psq[WHITE][pt][i];
//  color ^= 8;
//  for (pt = PAWN; pt <= KING; pt++)
//    for (i = 0; i < pcs[color + pt]; i++)
//      key ^= Zobrist::psq[BLACK][pt][i];

  return key;
}

// probe_wdl_table and probe_dtz_table require similar adaptations.
//static
int probe_wdl_table(Position& pos, int *success)
{
  struct TBEntry *ptr;
  struct TBHashEntry *ptr2;
  uint64 idx;
  uint64 key;
  int i;
  ubyte res;
  int p[TBPIECES];

  // Obtain the position's material signature key.
//  key = pos.material_key();
  key = calc_key(pos, 0);
//  key = pos.hash();

  // Test for KvK.
  if (key == (piece_square_hash[6][0] ^ piece_square_hash[12][0]))//(Zobrist::psq[WHITE][KING][0] ^ Zobrist::psq[BLACK][KING][0]))
    return 0;

  auto foo = key >> (64 - TBHASHBITS);
  ptr2 = TB_hash[foo];
  for (i = 0; i < HSHMAX; i++)
    if (ptr2[i].key == key) break;
  if (i == HSHMAX) {
    *success = 0;
    return 0;
  }

  ptr = ptr2[i].ptr;
  if (!ptr->ready) {
    LOCK(TB_mutex);
    if (!ptr->ready) {
      char str[16];
      prt_str(pos, str, ptr->key != key);
      if (!init_table_wdl(ptr, str)) {
	ptr2[i].key = 0ULL;
	*success = 0;
	UNLOCK(TB_mutex);
	return 0;
      }
      // Memory barrier to ensure ptr->ready = 1 is not reordered.
      __asm__ __volatile__ ("" ::: "memory");
      ptr->ready = 1;
    }
    UNLOCK(TB_mutex);
  }

  int bside, mirror, cmirror;
  if (!ptr->symmetric) {
    if (key != ptr->key) {
      cmirror = 8;
      mirror = 0x38;
      bside = (pos.color() == white);
    } else {
      cmirror = mirror = 0;
      bside = !(pos.color() == white);
    }
  } else {
    cmirror = pos.color() == white ? 0 : 8;
    mirror = pos.color() == white ? 0 : 0x38;
    bside = 0;
  }

  const board_t bitboards[2][6] =
  {
	  {
		  pos.occupy<pawn_tag, white_tag>(),
		  pos.occupy<knight_tag, white_tag>(),
		  pos.occupy<bishop_tag, white_tag>(),
		  pos.occupy<rook_tag, white_tag>(),
		  pos.occupy<queen_tag, white_tag>(),
		  pos.occupy<king_tag, white_tag>()
	  },
	  {
		  pos.occupy<pawn_tag, black_tag>(),
		  pos.occupy<knight_tag, black_tag>(),
		  pos.occupy<bishop_tag, black_tag>(),
		  pos.occupy<rook_tag, black_tag>(),
		  pos.occupy<queen_tag, black_tag>(),
		  pos.occupy<king_tag, black_tag>()
	  }
  };

  // p[i] is to contain the square 0-63 (A1-H8) for a piece of type
  // pc[i] ^ cmirror, where 1 = white pawn, ..., 14 = black king.
  // Pieces of the same type are guaranteed to be consecutive.
  if (!ptr->has_pawns) {
    struct TBEntry_piece *entry = (struct TBEntry_piece *)ptr;
    ubyte *pc = entry->pieces[bside];
    for (i = 0; i < entry->num;) {
    	board_t bb = bitboards[pc[i] ^ cmirror >> 3][pc[i] & 0x07];
    	for (square_t s : bsf(bb))
    		p[i++] = s;
//      Bitboard bb = pos.pieces((Color)((pc[i] ^ cmirror) >> 3),
//				      (PieceType)(pc[i] & 0x07));
//      do {
//	p[i++] = pop_lsb(&bb);
//      } while (bb);
    }
    idx = encode_piece(entry, entry->norm[bside], p, entry->factor[bside]);
    res = decompress_pairs(entry->precomp[bside], idx);
  } else {
    struct TBEntry_pawn *entry = (struct TBEntry_pawn *)ptr;
    int k = entry->file[0].pieces[0][0] ^ cmirror;
//    Bitboard bb = pos.pieces((Color)(k >> 3), (PieceType)(k & 0x07));
    i = 0;
//    do {
//      p[i++] = pop_lsb(&bb) ^ mirror;
//    } while (bb);
	board_t bb = bitboards[k >> 3][k & 7];
	for (square_t s : bsf(bb))
		p[i++] = s ^ mirror;

    int f = pawn_file(entry, p);
    ubyte *pc = entry->file[f].pieces[bside];
    for (; i < entry->num;) {
    	board_t bb = bitboards[(pc[i] ^ cmirror) >> 3][pc[i] & 7];
    	for (square_t s : bsf(bb))
    		p[i++] = s ^ mirror;
//      bb = pos.pieces((Color)((pc[i] ^ cmirror) >> 3),
//				    (PieceType)(pc[i] & 0x07));
//      do {
//	p[i++] = pop_lsb(&bb) ^ mirror;
//      } while (bb);
    }
    idx = encode_pawn(entry, entry->file[f].norm[bside], p, entry->file[f].factor[bside]);
    res = decompress_pairs(entry->file[f].precomp[bside], idx);
  }

  return ((int)res) - 2;
}

// The value of wdl MUST correspond to the WDL value of the position without
// en passant rights.
static int probe_dtz_table(Position& pos, int wdl, int *success)
{
  struct TBEntry *ptr;
  uint64 idx;
  int i, res;
  int p[TBPIECES];

  // Obtain the position's material signature key.
//  uint64 key = pos.material_key();
  uint64 key = calc_key(pos, 0);

  if (DTZ_table[0].key1 != key && DTZ_table[0].key2 != key) {
    for (i = 1; i < DTZ_ENTRIES; i++)
      if (DTZ_table[i].key1 == key || DTZ_table[i].key2 == key) break;
    if (i < DTZ_ENTRIES) {
      struct DTZTableEntry table_entry = DTZ_table[i];
      for (; i > 0; i--)
	DTZ_table[i] = DTZ_table[i - 1];
      DTZ_table[0] = table_entry;
    } else {
      struct TBHashEntry *ptr2 = TB_hash[key >> (64 - TBHASHBITS)];
      for (i = 0; i < HSHMAX; i++)
	if (ptr2[i].key == key) break;
      if (i == HSHMAX) {
	*success = 0;
	return 0;
      }
      ptr = ptr2[i].ptr;
      char str[16];
      int mirror = (ptr->key != key);
      prt_str(pos, str, mirror);
      if (DTZ_table[DTZ_ENTRIES - 1].entry)
	free_dtz_entry(DTZ_table[DTZ_ENTRIES-1].entry);
      for (i = DTZ_ENTRIES - 1; i > 0; i--)
	DTZ_table[i] = DTZ_table[i - 1];
      load_dtz_table(str, calc_key(pos, mirror), calc_key(pos, !mirror));
    }
  }

  ptr = DTZ_table[0].entry;
  if (!ptr) {
    *success = 0;
    return 0;
  }

  int bside, mirror, cmirror;
  if (!ptr->symmetric) {
    if (key != ptr->key) {
      cmirror = 8;
      mirror = 0x38;
      bside = (pos.color() == white);
    } else {
      cmirror = mirror = 0;
      bside = !(pos.color() == white);
    }
  } else {
    cmirror = pos.color() == white ? 0 : 8;
    mirror = pos.color() == white ? 0 : 0x38;
    bside = 0;
  }

  const board_t bitboards[14]
  {
	  pos.occupy<pawn_tag, white_tag>(),
	  pos.occupy<knight_tag, white_tag>(),
	  pos.occupy<bishop_tag, white_tag>(),
	  pos.occupy<rook_tag, white_tag>(),
	  pos.occupy<queen_tag, white_tag>(),
	  pos.occupy<king_tag, white_tag>(),
	  0,
	  0,
	  pos.occupy<pawn_tag, black_tag>(),
	  pos.occupy<knight_tag, black_tag>(),
	  pos.occupy<bishop_tag, black_tag>(),
	  pos.occupy<rook_tag, black_tag>(),
	  pos.occupy<queen_tag, black_tag>(),
	  pos.occupy<king_tag, black_tag>()
  };

  if (!ptr->has_pawns) {
    struct DTZEntry_piece *entry = (struct DTZEntry_piece *)ptr;
    if ((entry->flags & 1) != bside && !entry->symmetric) {
      *success = -1;
      return 0;
    }
    ubyte *pc = entry->pieces;
    for (i = 0; i < entry->num;) {
//      Bitboard bb = pos.pieces((Color)((pc[i] ^ cmirror) >> 3),
//				    (PieceType)(pc[i] & 0x07));
//      do {
//	p[i++] = pop_lsb(&bb);
//      } while (bb);
    	board_t bb = bitboards[pc[i] ^ cmirror];
    	for (square_t s : bsf(bb))
    		p[i++] = s;
    }
    idx = encode_piece((struct TBEntry_piece *)entry, entry->norm, p, entry->factor);
    res = decompress_pairs(entry->precomp, idx);

    if (entry->flags & 2)
      res = entry->map[entry->map_idx[wdl_to_map[wdl + 2]] + res];

    if (!(entry->flags & pa_flags[wdl + 2]) || (wdl & 1))
      res *= 2;
  } else {
    struct DTZEntry_pawn *entry = (struct DTZEntry_pawn *)ptr;
    int k = entry->file[0].pieces[0] ^ cmirror;
//    Bitboard bb = pos.pieces((Color)(k >> 3), (PieceType)(k & 0x07));
//    i = 0;
//    do {
//      p[i++] = pop_lsb(&bb) ^ mirror;
//    } while (bb);
	board_t bb = bitboards[k];
	for (square_t s : bsf(bb))
		p[i++] = s ^ mirror;
    int f = pawn_file((struct TBEntry_pawn *)entry, p);
    if ((entry->flags[f] & 1) != bside) {
      *success = -1;
      return 0;
    }
    ubyte *pc = entry->file[f].pieces;
    for (; i < entry->num;) {
////      bb = pos.pieces((Color)((pc[i] ^ cmirror) >> 3),
////			    (PieceType)(pc[i] & 0x07));
////      do {
////	p[i++] = pop_lsb(&bb) ^ mirror;
//      } while (bb);
    	board_t bb = bitboards[pc[i] ^ cmirror];
    	for (square_t s : bsf(bb))
    		p[i++] = s ^ mirror;
    }
    idx = encode_pawn((struct TBEntry_pawn *)entry, entry->file[f].norm, p, entry->file[f].factor);
    res = decompress_pairs(entry->file[f].precomp, idx);

    if (entry->flags[f] & 2)
      res = entry->map[entry->map_idx[f][wdl_to_map[wdl + 2]] + res];

    if (!(entry->flags[f] & pa_flags[wdl + 2]) || (wdl & 1))
      res *= 2;
  }

  return res;
}
/*
// Add underpromotion captures to list of captures.
static ExtMove *add_underprom_caps(Position& pos, MoveStack *stack, MoveStack *end)
{
  ExtMove *moves, *extra = end;

  for (moves = stack; moves < end; moves++) {
    Move move = moves->move;
    if (type_of(move) == PROMOTION && !pos.empty(to_sq(move))) {
      (*extra++).move = (Move)(move - (1 << 12));
      (*extra++).move = (Move)(move - (2 << 12));
      (*extra++).move = (Move)(move - (3 << 12));
    }
  }

  return extra;
}

static int probe_ab(Position& pos, int alpha, int beta, int *success)
{
  int v;
  ExtMove stack[64];
  ExtMove *moves, *end;
  StateInfo st;

  // Generate (at least) all legal captures including (under)promotions.
  // It is OK to generate more, as long as they are filtered out below.
  if (!pos.checkers()) {
    end = generate<CAPTURES>(pos, stack);
    // Since underpromotion captures are not included, we need to add them.
    end = add_underprom_caps(pos, stack, end);
  } else
    end = generate<EVASIONS>(pos, stack);

  CheckInfo ci(pos);

  for (moves = stack; moves < end; moves++) {
    Move capture = moves->move;
    if (!pos.capture(capture) || !pos.legal(capture, ci.pinned))
      continue;
    pos.do_move(capture, st, ci, pos.gives_check(capture, ci));
    v = -probe_ab(pos, -beta, -alpha, success);
    pos.undo_move(capture);
    if (*success == 0) return 0;
    if (v > alpha) {
      if (v >= beta)
	return v;
      alpha = v;
    }
  }

  v = probe_wdl_table(pos, success);

  return alpha >= v ? alpha : v;
}

// Probe the WDL table for a particular position.
//
// If *success != 0, the probe was successful.
//
// If *success == 2, the position has a winning capture, or the position
// is a cursed win and has a cursed winning capture, or the position
// has an ep capture as only best move.
// This is used in probe_dtz().
//
// The return value is from the point of view of the side to move:
// -2 : loss
// -1 : loss, but draw under 50-move rule
//  0 : draw
//  1 : win, but draw under 50-move rule
//  2 : win
int probe_wdl(Position& pos, int *success)
{
  *success = 1;

  // Generate (at least) all legal en passant captures.
  ExtMove stack[192];
  ExtMove *moves, *end;
  StateInfo st;

  // Generate (at least) all legal captures including (under)promotions.
  if (!pos.checkers()) {
    end = generate<CAPTURES>(pos, stack);
    end = add_underprom_caps(pos, stack, end);
  } else
    end = generate<EVASIONS>(pos, stack);

  CheckInfo ci(pos);

  int best_cap = -3, best_ep = -3;

  // We do capture resolution, letting best_cap keep track of the best
  // capture without ep rights and letting best_ep keep track of still
  // better ep captures if they exist.

  for (moves = stack; moves < end; moves++) {
    Move capture = moves->move;
    if (!pos.capture(capture) || !pos.legal(capture, ci.pinned))
      continue;
    pos.do_move(capture, st, ci, pos.gives_check(capture, ci));
    int v = -probe_ab(pos, -2, -best_cap, success);
    pos.undo_move(capture);
    if (*success == 0) return 0;
    if (v > best_cap) {
      if (v == 2) {
	*success = 2;
	return 2;
      }
      if (type_of(capture) != ENPASSANT)
        best_cap = v;
      else if (v > best_ep)
        best_ep = v;
    }
  }

  int v = probe_wdl_table(pos, success);
  if (*success == 0) return 0;

  // Now max(v, best_cap) is the WDL value of the position without ep rights.
  // If the position without ep rights is not stalemate or no ep captures
  // exist, then the value of the position is max(v, best_cap, best_ep).
  // If the position without ep rights is stalemate and best_ep > -3,
  // then the value of the position is best_ep (and we will have v == 0).

  if (best_ep > best_cap) {
    if (best_ep > v) { // ep capture (possibly cursed losing) is best.
      *success = 2;
      return best_ep;
    }
    best_cap = best_ep;
  }

  // Now max(v, best_cap) is the WDL value of the position unless
  // the position without ep rights is stalemate and best_ep > -3.

  if (best_cap >= v) {
    // No need to test for the stalemate case here: either there are
    // non-ep captures, or best_cap == best_ep >= v anyway.
    *success = 1 + (best_cap > 0);
    return best_cap;
  }

  // Now handle the stalemate case.
  if (best_ep > -3 && v == 0) {
    // Check for stalemate in the position with ep captures.
    for (moves = stack; moves < end; moves++) {
      Move move = moves->move;
      if (type_of(move) == ENPASSANT) continue;
      if (pos.legal(move, ci.pinned)) break;
    }
    if (moves == end && !pos.checkers()) {
      end = generate<QUIETS>(pos, end);
      for (; moves < end; moves++) {
        Move move = moves->move;
        if (pos.legal(move, ci.pinned))
          break;
      }
    }
    if (moves == end) { // Stalemate detected.
      *success = 2;
      return best_ep;
    }
  }

  // Stalemate / en passant not an issue, so v is the correct value.

  return v;
}

static int wdl_to_dtz[] = {
  -1, -101, 0, 101, 1
};

// Probe the DTZ table for a particular position.
// If *success != 0, the probe was successful.
// The return value is from the point of view of the side to move:
//         n < -100 : loss, but draw under 50-move rule
// -100 <= n < -1   : loss in n ply (assuming 50-move counter == 0)
//         0	    : draw
//     1 < n <= 100 : win in n ply (assuming 50-move counter == 0)
//   100 < n        : win, but draw under 50-move rule
//
// If the position is mate, -1 is returned instead of 0.
//
// The return value n can be off by 1: a return value -n can mean a loss
// in n+1 ply and a return value +n can mean a win in n+1 ply. This
// cannot happen for tables with positions exactly on the "edge" of
// the 50-move rule.
//
// This means that if dtz > 0 is returned, the position is certainly
// a win if dtz + 50-move-counter <= 99. Care must be taken that the engine
// picks moves that preserve dtz + 50-move-counter <= 99.
//
// If n = 100 immediately after a capture or pawn move, then the position
// is also certainly a win, and during the whole phase until the next
// capture or pawn move, the inequality to be preserved is
// dtz + 50-movecounter <= 100.
//
// In short, if a move is available resulting in dtz + 50-move-counter <= 99,
// then do not accept moves leading to dtz + 50-move-counter == 100.
//
int probe_dtz(Position& pos, int *success)
{
  int wdl = probe_wdl(pos, success);
  if (*success == 0) return 0;

  // If draw, then dtz = 0.
  if (wdl == 0) return 0;

  // Check for winning (cursed) capture or ep capture as only best move.
  if (*success == 2)
    return wdl_to_dtz[wdl + 2];

  ExtMove stack[192];
  ExtMove *moves, *end = NULL;
  StateInfo st;
  CheckInfo ci(pos);

  // If winning, check for a winning pawn move.
  if (wdl > 0) {
    // Generate at least all legal non-capturing pawn moves
    // including non-capturing promotions.
    // (The call to generate<>() in fact generates all moves.)
    if (!pos.checkers())
      end = generate<NON_EVASIONS>(pos, stack);
    else
      end = generate<EVASIONS>(pos, stack);

    for (moves = stack; moves < end; moves++) {
      Move move = moves->move;
      if (type_of(pos.moved_piece(move)) != PAWN || pos.capture(move)
                || !pos.legal(move, ci.pinned))
        continue;
      pos.do_move(move, st, ci, pos.gives_check(move, ci));
      int v = -probe_wdl(pos, success);
      pos.undo_move(move);
      if (*success == 0) return 0;
      if (v == wdl)
        return wdl_to_dtz[wdl + 2];
    }
  }

  // If we are here, we know that the best move is not an ep capture.
  // In other words, the value of wdl corresponds to the WDL value of
  // the position without ep rights. It is therefore safe to probe the
  // DTZ table with the current value of wdl.

  int dtz = probe_dtz_table(pos, wdl, success);
  if (*success >= 0)
    return wdl_to_dtz[wdl + 2] + ((wdl > 0) ? dtz : -dtz);

  // *success < 0 means we need to probe DTZ for the other side to move.
  int best;
  if (wdl > 0) {
    best = INT32_MAX;
    // If wdl > 0, we already generated all moves.
  } else {
    // If (cursed) loss, the worst case is a losing capture or pawn move
    // as the "best" move, leading to dtz of -1 or -101.
    // In case of mate, this will cause -1 to be returned.
    best = wdl_to_dtz[wdl + 2];
    if (!pos.checkers())
      end = generate<NON_EVASIONS>(pos, stack);
    else
      end = generate<EVASIONS>(pos, stack);
  }

  for (moves = stack; moves < end; moves++) {
    Move move = moves->move;
    // We can skip pawn moves and captures.
    // If wdl > 0, we already caught them. If wdl < 0, the initial value
    // of best already takes account of them.
    if (pos.capture(move) || type_of(pos.moved_piece(move)) == PAWN
              || !pos.legal(move, ci.pinned))
      continue;
    pos.do_move(move, st, ci, pos.gives_check(move, ci));
    int v = -probe_dtz(pos, success);
    pos.undo_move(move);
    if (*success == 0) return 0;
    if (wdl > 0) {
      if (v > 0 && v + 1 < best)
        best = v + 1;
    } else {
      if (v -1 < best)
        best = v - 1;
    }
  }
  return best;
}

// Check whether there has been at least one repetition of positions
// since the last capture or pawn move.
static int has_repeated(StateInfo *st)
{
  while (1) {
    int i = 4, e = std::min(st->rule50, st->pliesFromNull);
    if (e < i)
      return 0;
    StateInfo *stp = st->previous->previous;
    do {
      stp = stp->previous->previous;
      if (stp->key == st->key)
	return 1;
      i += 2;
    } while (i <= e);
    st = st->previous;
  }
}

static Value wdl_to_value[5] = {
  -VALUE_MATE + MAX_PLY + 1,
  VALUE_DRAW - 2,
  VALUE_DRAW,
  VALUE_DRAW + 2,
  VALUE_MATE - MAX_PLY - 1
};

// Use the DTZ tables to filter out moves that don't preserve the win or draw.
// If the position is lost, but DTZ is fairly high, only keep moves that
// maximise DTZ.
//
// A return value of 0 indicates that not all probes were successful and that
// no moves were filtered out.
int root_probe(Position& pos, Value &TBScore)
{
  int success;

  int dtz = probe_dtz(pos, &success);
  if (!success) return 0;

  StateInfo st;
  CheckInfo ci(pos);

  // Probe each move.
  for (size_t i = 0; i < Search::RootMoves.size(); i++) {
    Move move = Search::RootMoves[i].pv[0];
    pos.do_move(move, st, ci, pos.gives_check(move, ci));
    int v = 0;
    if (pos.checkers() && dtz > 0) {
      ExtMove s[192];
      if (generate<LEGAL>(pos, s) == s)
	v = 1;
    }
    if (!v) {
      if (st.rule50 != 0) {
	v = -probe_dtz(pos, &success);
	if (v > 0) v++;
	else if (v < 0) v--;
      } else {
	v = -probe_wdl(pos, &success);
	v = wdl_to_dtz[v + 2];
      }
    }
    pos.undo_move(move);
    if (!success) return 0;
    Search::RootMoves[i].score = (Value)v;
  }

  // Obtain 50-move counter for the root position.
  // In Stockfish there seems to be no clean way, so we do it like this:
  int cnt50 = st.previous->rule50;

  // Use 50-move counter to determine whether the root position is
  // won, lost or drawn.
  int wdl = 0;
  if (dtz > 0)
    wdl = (dtz + cnt50 <= 100) ? 2 : 1;
  else if (dtz < 0)
    wdl = (-dtz + cnt50 <= 100) ? -2 : -1;

  // Determine the score to report to the user.
  TBScore = wdl_to_Value[wdl + 2];
  // If the position is winning or losing, but too few moves left, adjust the
  // score to show how close it is to winning or losing.
  // NOTE: int(PawnValueEg) is used as scaling factor in score_to_uci().
  if (wdl == 1 && dtz <= 100)
    TBScore = (Value)(((200 - dtz - cnt50) * int(PawnValueEg)) / 200);
  else if (wdl == -1 && dtz >= -100)
    TBScore = -(Value)(((200 + dtz - cnt50) * int(PawnValueEg)) / 200);

  // Now be a bit smart about filtering out moves.
  size_t j = 0;
  if (dtz > 0) { // winning (or 50-move rule draw)
    int best = 0xffff;
    for (size_t i = 0; i < Search::RootMoves.size(); i++) {
      int v = Search::RootMoves[i].score;
      if (v > 0 && v < best)
	best = v;
    }
    int max = best;
    // If the current phase has not seen repetitions, then try all moves
    // that stay safely within the 50-move budget, if there are any.
    if (!has_repeated(st.previous) && best + cnt50 <= 99)
      max = 99 - cnt50;
    for (size_t i = 0; i < Search::RootMoves.size(); i++) {
      int v = Search::RootMoves[i].score;
      if (v > 0 && v <= max)
	Search::RootMoves[j++] = Search::RootMoves[i];
    }
  } else if (dtz < 0) {
    int best = 0;
    for (size_t i = 0; i < Search::RootMoves.size(); i++) {
      int v = Search::RootMoves[i].score;
      if (v < best)
	best = v;
    }
    // Try all moves, unless we approach or have a 50-move rule draw.
    if (-best * 2 + cnt50 < 100)
      return 1;
    for (size_t i = 0; i < Search::RootMoves.size(); i++) {
      if (Search::RootMoves[i].score == best)
	Search::RootMoves[j++] = Search::RootMoves[i];
    }
  } else { // drawing
    // Try all moves that preserve the draw.
    for (size_t i = 0; i < Search::RootMoves.size(); i++) {
      if (Search::RootMoves[i].score == 0)
	Search::RootMoves[j++] = Search::RootMoves[i];
    }
  }
  Search::RootMoves.resize(j, Search::RootMove(MOVE_NONE));

  return 1;
}

// Use the WDL tables to filter out moves that don't preserve the win or draw.
// This is a fallback for the case that some or all DTZ tables are missing.
//
// A return value of 0 indicates that not all probes were successful and that
// no moves were filtered out.
int root_probe_wdl(Position& pos, Value& TBScore)
{
  int success;

  int wdl = probe_wdl(pos, &success);
  if (!success) return false;
  TBScore = wdl_to_Value[wdl + 2];

  StateInfo st;
  CheckInfo ci(pos);

  int best = -2;

  // Probe each move.
  for (size_t i = 0; i < Search::RootMoves.size(); i++) {
    Move move = Search::RootMoves[i].pv[0];
    pos.do_move(move, st, ci, pos._check(move, ci));
    int v = -probe_wdl(pos, &success);
    pos.undo_move(move);
    if (!success) return false;
    Search::RootMoves[i].score = (Value)v;
    if (v > best)
      best = v;
  }

  size_t j = 0;
  for (size_t i = 0; i < Search::RootMoves.size(); i++) {
    if (Search::RootMoves[i].score == best)
      Search::RootMoves[j++] = Search::RootMoves[i];
  }
  Search::RootMoves.resize(j, Search::RootMove(MOVE_NONE));

  return 1;
}
*/
}

//using chess::calc_key_from_pcs;
