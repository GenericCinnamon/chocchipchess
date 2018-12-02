#include <string>
#include "bitops.h"
#include <intrin.h>

using namespace std;

/*
            north

most significant bit
    ->    1 0 0 0 0 0 1 0
        0 0 0 0 0 0 0 0
        0 1 0 0 0 0 1 0
  west    0 0 0 0 0 0 0 0  east
        0 0 0 0 0 0 0 0
        1 1 1 0 0 0 0 0
        0 0 0 0 1 0 0 0
        0 0 0 0 0 0 0 1 <- 
                    least significant bit
             south
*/


/*
log2_64 - fast log base 2 for 64 bit integers
returns the position of the highest set bit
*/
uint8_t log2_64(uint64_t value)
{
    return 63 - __lzcnt64(value);
}

/*
log2andclear_64 - log base 2 and clear the highest set bit
*/
uint8_t log2andclear_64(uint64_t &value)
{
    uint8_t pos = log2_64(value);
    value ^= 1ULL << pos;
    return pos;
}

/*
clear_bit - set a bit to zero
*/
void clear_bit(u64 & value, int pos)
{
  value &= ~(1ULL << pos);
}

/*
set_bit - set a bit to 1
*/
void set_bit(u64 & value, int pos)
{
  value |= 1ULL << pos;
}

/*
debugPrintBitBoard - print a 64 bit int to stdout in a 8x8 pattern
*/
string debugPrintBitBoard(uint64_t value)
{
    string result = "";
    char output[65];
    output[64] = 0;
    result += "\n";
    for (uint8_t i = 0; i < 64; i++) {
        if (i && i % 8 == 0)
            result += "\n";
        int row = 7 - i / 8;
        int col = i % 8;
        result += value & (1ULL << (row * 8 + col)) ? '1' : '0';
    }
    result += "\n";

    return result;
}

/*
shift_north - shift all bits up by 8 i.e. to the next row
*/
void shift_north(u64 &position)
{
    position <<= 8;
}

/*
shift_east - shift left by 1
*/
void shift_east(u64 &position)
{
    position <<= 1;
}

void shift_south(u64 &position)
{
    position >>= 8;
}

void shift_west(u64 &position)
{
    position >>= 1;
}

void shift_north_east(u64 &position)
{
    shift_north(position);
    shift_east(position);
}

void shift_south_east(u64 &position)
{
    shift_south(position);
    shift_east(position);
}

void shift_south_west(u64 &position)
{
    shift_south(position);
    shift_west(position);
}

void shift_north_west(u64 &position)
{
    shift_north(position);
    shift_west(position);
}

void shift_inplace(u64& position, int amount) {
    if (amount < 0)
        position >>= -amount;
    else
        position <<= amount;
}

u64 shift_by(u64 position, int amount)
{
    if (amount < 0)
        return position >> -amount;
    else
        return position << amount;
}
