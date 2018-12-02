#pragma once
#include <inttypes.h>

#define u64 uint64_t

const bool WHITE = 0;
const bool BLACK = 1;

const int CHECKMATE_VALUE = 300000;
const int CHECKMATE_THRESHOLD = CHECKMATE_VALUE - 100;

inline int sideSign(bool side) { return 1 - 2 * side; };

enum PIECES {
    WHITE_PAWN,
    WHITE_ROOK,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_ROOK,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_QUEEN,
    BLACK_KING,
    WHITE_PIECES,
    BLACK_PIECES,
    EN_PASSENT,
    BIT_BOARD_COUNT
};

const char PIECE_CHARS[] = "PRNBQKprnbqk";

const u64 FULLBOARDBB = 0xffffffffffffffff;

const char STARTPOS_FEN[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const int NORTH = 8;
const int EAST = 1;
const int SOUTH = -8;
const int WEST = -1;

const int NORTH_EAST = NORTH + EAST;
const int SOUTH_EAST = SOUTH + EAST;
const int SOUTH_WEST = SOUTH + WEST;
const int NORTH_WEST = NORTH + WEST;

const u64 RANK1 = 0xff;
const u64 RANK2 = RANK1 << 8;
const u64 RANK3 = RANK2 << 8;
const u64 RANK4 = RANK3 << 8;
const u64 RANK5 = RANK4 << 8;
const u64 RANK6 = RANK5 << 8;
const u64 RANK7 = RANK6 << 8;
const u64 RANK8 = RANK7 << 8;

const u64 FILEA = 0x0101010101010101ULL;
const u64 FILEB = FILEA << 1;
const u64 FILEC = FILEB << 1;
const u64 FILED = FILEC << 1;
const u64 FILEE = FILED << 1;
const u64 FILEF = FILEE << 1;
const u64 FILEG = FILEF << 1;
const u64 FILEH = FILEG << 1;

const char WHITE_KINGSIDE = 1;
const char WHITE_QUEENSIDE = 2;
const char WHITE_CASTLING = 3;
const char BLACK_KINGSIDE = 4;
const char BLACK_QUEENSIDE = 8;
const char BLACK_CASTLING = 12;

const int NO_ENPASSENT = 64;

enum SQUARES {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
};


struct MoveDetails {
    int shift_by;
    u64 mask;
};

const MoveDetails KING_MOVES[] = {
    { NORTH, RANK8 },
    { NORTH + EAST, FILEH | RANK8 },
    { EAST, FILEH },
    { SOUTH + EAST, FILEH | RANK1 },
    { SOUTH, RANK1 },
    { SOUTH + WEST, FILEA | RANK1 },
    { WEST, FILEA },
    { NORTH + WEST, FILEA | RANK8 }
};

const MoveDetails ROOK_MOVES[] = {
    { NORTH, RANK8 },
    { EAST, FILEH },
    { SOUTH, RANK1 },
    { WEST, FILEA },
};

const MoveDetails BISHOP_MOVES[] = {
    { NORTH + EAST, FILEH | RANK8 },
    { SOUTH + EAST, FILEH | RANK1 },
    { SOUTH + WEST, FILEA | RANK1 },
    { NORTH + WEST, FILEA | RANK8 }
};

const MoveDetails QUEEN_MOVES[] = {
    { NORTH, RANK8},
    { NORTH + EAST, FILEH | RANK8 },
    { EAST, FILEH },
    { SOUTH + EAST, FILEH | RANK1 },
    { SOUTH, RANK1 },
    { SOUTH + WEST, FILEA | RANK1 },
    { WEST, FILEA },
    { NORTH + WEST, FILEA | RANK8 }
};

const MoveDetails KNIGHT_MOVES[] = {
    { NORTH + NORTH + EAST, FILEH | RANK7 | RANK8 },
    { EAST + NORTH + EAST, FILEG | FILEH | RANK8 },
    { EAST + SOUTH + EAST, FILEG | FILEH | RANK1 },
    { SOUTH + SOUTH + EAST, FILEH | RANK1 | RANK2 },
    { SOUTH + SOUTH + WEST, FILEA | RANK1 | RANK2 },
    { WEST + SOUTH + WEST, FILEA | FILEB | RANK1 },
    { WEST + NORTH + WEST, FILEA | FILEB | RANK8 },
    { NORTH + NORTH + WEST, FILEA | RANK7 | RANK8 }
};
