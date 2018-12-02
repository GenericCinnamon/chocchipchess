#pragma once
#include <vector>

#include "move.h"
#include "position.h"


class Search
{
public:
    Search();
    void ponderPosition(Position& position);
    int stop();

private:
    int negamax(Position& position, int depth, int alpha, int beta);

    void Search::ponderPosition(Position& position, int depth);

    bool stopThinking;
    Move* topMove();

    void ponder(int depth);
    void getPV(vector<Move>* move_list, int depth);

    class exit_search : exception {};
};

