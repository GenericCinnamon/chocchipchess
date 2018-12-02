#include "PositionExtras.h"
#include "gtest\gtest.h"


PositionExtras::PositionExtras(string fen) : Position(fen)
{
}


int PositionExtras::perft(int depth)
{
    if (depth == 0) return 1;

    int count = 0;
    for (auto move : genMoves()) {
        performMove(&move);
        count += perft(depth - 1);
        undoMove(&move);
    }

    return count;
}

int PositionExtras::perftWithTT(int depth, unordered_map<uint64_t, string> hash_to_fen)
{
    if (depth == 0) return 1;
    
    int count = 0;

    if (zobristHash != calculateZobristHash())
        throw new exception("Incorrect zobrist hash");

    for (auto move : genMoves()) {
        performMove(&move);
        auto found_fen = hash_to_fen.find(zobristHash);

        if (found_fen != hash_to_fen.end())
            if (toFEN() != found_fen->second)
                throw new exception("key collision");
        else
            hash_to_fen[zobristHash] = toFEN();
        
        throwIfInvalid();
        count += perftWithTT(depth - 1, hash_to_fen);

        undoMove(&move);
    }

    return count;
}

void PositionExtras::throwIfInvalid()
{
    u64 total;

    for (int i = 0; i < 11; i++)
        for (int j = i + 1; j < 12; j++)
            if (bitBoards[i] & bitBoards[j])
                throw logic_error("Board has overlapping pieces.");

    total = 0;
    for (int i = 0; i < 6; i++) {
        total |= bitBoards[i];
        if (bitBoards[i] & ~bitBoards[WHITE_PIECES])
            throw logic_error("White pieces are out of sync with their union board.");
    }

    if (total ^ bitBoards[WHITE_PIECES])
        throw new std::logic_error("White pieces are out of sync with their union board.");

    total = 0;
    for (int i = 6; i < 12; i++) {
        total |= bitBoards[i];
        if (bitBoards[i] & ~bitBoards[BLACK_PIECES])
            throw new std::logic_error("Black pieces are out of sync with their union board.");
    }

    if (total ^ bitBoards[BLACK_PIECES])
        throw new std::logic_error("Black pieces are out of sync with their union board.");

    if (zobristHash != calculateZobristHash())
        throw new std::logic_error("Incorrect zobrist hash");
}

