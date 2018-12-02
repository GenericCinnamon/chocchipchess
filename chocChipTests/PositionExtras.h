#pragma once
#include "../chocChipChess/position.h"
#include <string>
#include <map>

class PositionExtras : public Position {
public:
    PositionExtras(string fen);
    int perft(int depth);
    int perftWithTT(int depth, unordered_map<uint64_t, string>);


    void throwIfInvalid();

    using Position::bitBoards;
    using Position::checkCheck;
    using Position::genMoves;
    using Position::alphabeta;
    using Position::negamax;
    using Position::performMove;
    using Position::undoMove;
    using Position::createMove;
    using Position::calculateZobristHash;
    using Position::zobristHash;
};