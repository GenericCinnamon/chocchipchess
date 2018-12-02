#pragma once
#include <cinttypes>
#include <string>
#include <vector>
#include <unordered_map>
#include <stack>

#include "move.h"
#include "board_definitions.h"
#include "transposition_table.h"
#include "output_helper.h"
#include "zobrist.h"

using namespace std;


class Position
{
protected:
    // Position info
    uint64_t bitBoards[BIT_BOARD_COUNT];

    // Move generation
    vector<Move> genMoves();
    void moveHopper(vector<Move>* move_list, int piece_type, const MoveDetails moves[], int arr_size);
    void moveSlider(vector<Move>* move_list, int piece_type, const MoveDetails moves[], int arr_size);
    void moveWhitePawns(vector<Move>* move_list);
    void moveBlackPawns(vector<Move>* move_list);
    void moveCastling(vector<Move>* move_list);
    bool checkCheck(bool side);

    void createMove(vector<Move>*, int from_square, int to_square, int piece);
    void createMove(vector<Move>*, int from_square, int to_square, int piece, int promotion_piece);
    void performMove(Move* move);
    void undoMove(Move* move);
    void updateCastlingRights();
    stack<char> castlingRightsStack;
    stack<char> enPassentStack;

    // Searching
    int calculateStaticEval();

    // Transposition Table
    u64 zobristHash;
    u64 calculateZobristHash();

    TranspositionTable* tt;
    int ttSize = 1000;

    OutputHelper* output;

public:
    Position(string position);
    Position(string position, TranspositionTable *shared_tt);
    
    // Position info
    bool sideToMove;
    char castlingRights;

    string printBoard();
    string printBitBoards();
    void loadFEN(string position);
    string toFEN();

    void directOutput(OutputHelper* helper);

    void updateSideBitBoards();

    bool inputMove(string move_name); 
    int staticEval;
    bool inCheck;
    unsigned int halfMoveCount;
    Move* makeMove(int depth);
    Move* makeMove();
};

