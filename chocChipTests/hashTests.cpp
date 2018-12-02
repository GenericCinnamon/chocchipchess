#include "gtest/gtest.h"
#include <map>
#include <string>
#include <inttypes.h>
#include <vector>

#include "PositionExtras.h"

using namespace std;

TEST(ZobristTests, PerftHashUniquenessCheck) {
    PositionExtras pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    unordered_map<uint64_t, string> tt;
    EXPECT_NO_THROW(pos.perftWithTT(3, tt));
}

TEST(ZobristTests, PerftHashUniquenessCheck2) {
    PositionExtras pos("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    pos.inputMove("e2e4");
    pos.inputMove("e7e5");
    pos.inputMove("g1f3");


    unordered_map<uint64_t, string> tt;
    EXPECT_NO_THROW(pos.perftWithTT(3, tt));
}

TEST(ZobirstTests, PerfHashUniquenessHalfGame) {
    PositionExtras pos("r1bqk1nr/p1ppbp1p/1p6/3Np1pQ/2P1P2P/8/PP1P1PP1/n1BK1BNR b kq - 1 9");
    unordered_map<uint64_t, string> tt;
    EXPECT_NO_THROW(pos.perftWithTT(3, tt));
}