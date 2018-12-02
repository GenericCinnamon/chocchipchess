#include <inttypes.h>
#include <random>

namespace Zobrist {
    uint64_t PIECE_INTS[12][64];
    uint64_t CASTLING_INTS[4];
    uint64_t SIDE_IS_BLACK_INT;
    uint64_t EN_PASSENT_INTS[8];

    void init(unsigned int seed) {
        std::default_random_engine generator;
        generator.seed(seed);
        std::uniform_int_distribution<uint64_t> distribution(1, UINT64_MAX);

        for (int i = 0; i < 12; i++)
            for (int j = 0; j < 64; j++)
                Zobrist::PIECE_INTS[i][j] = distribution(generator);

        for (int i = 0; i < 4; i++)
            Zobrist::CASTLING_INTS[i] = distribution(generator);

        Zobrist::SIDE_IS_BLACK_INT = distribution(generator);

        for (int i = 0; i < 8; i++)
            Zobrist::EN_PASSENT_INTS[i] = distribution(generator);
    }
}