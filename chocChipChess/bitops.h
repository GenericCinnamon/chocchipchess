#pragma once
#include <string>
#include <inttypes.h>

#define u64 uint64_t

using namespace std;

const int tab64[64] = {
    63,  0, 58,  1, 59, 47, 53,  2,
    60, 39, 48, 27, 54, 33, 42,  3,
    61, 51, 37, 40, 49, 18, 28, 20,
    55, 30, 34, 11, 43, 14, 22,  4,
    62, 57, 46, 52, 38, 26, 32, 41,
    50, 36, 17, 19, 29, 10, 13, 21,
    56, 45, 25, 31, 35, 16,  9, 12,
    44, 24, 15,  8, 23,  7,  6,  5 };


uint8_t log2_64(uint64_t value);

uint8_t log2andclear_64(uint64_t& value);

void clear_bit(u64& value, int position);
void set_bit(u64& value, int position);

string debugPrintBitBoard(uint64_t value);

void shift_north(u64& position);
void shift_east(u64& position);
void shift_south(u64& position);
void shift_west(u64& position);

void shift_north_east(u64& position);
void shift_south_east(u64& position);
void shift_south_west(u64& position);
void shift_north_west(u64& position);

void shift_inplace(u64& position, int amount);
u64 shift_by(u64 position, int amount);
