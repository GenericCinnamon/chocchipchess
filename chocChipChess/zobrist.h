#pragma once

#include <inttypes.h>

namespace Zobrist {
    extern uint64_t PIECE_INTS[12][64];
    extern uint64_t CASTLING_INTS[4];
    extern uint64_t SIDE_IS_BLACK_INT;
    extern uint64_t EN_PASSENT_INTS[8];

    void init(unsigned int seed);
}
