#include "gtest/gtest.h"
#include "../chocChipChess/bitops.h"
#include <inttypes.h>


TEST(BitopsTests, FastLogs) {
    EXPECT_EQ(log2_64(1ULL), (uint8_t)0ULL);
    EXPECT_EQ(log2_64(129ULL), (int8_t)7ULL);
    EXPECT_EQ(log2_64(0xffffffffffffffff), (uint8_t)63ULL);
}

TEST(BitopsTests, FastLogAndClear) {
    uint64_t temp = 256ULL;
    uint8_t result = log2andclear_64(temp);

    EXPECT_EQ(result, (uint8_t) 8);
    EXPECT_EQ(temp, (uint64_t) 0);
}
