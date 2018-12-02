#pragma once
#include <string>
#include <inttypes.h>

using namespace std;

/*
Representation of a move on the chess board
*/
class Move
{
public:
    int fromSquare;
    int toSquare;
    int pieceMoved;
    int pieceTaken;
    int promotionPiece;
    int eval;
    int depth;

    Move(int from_square, int to_square, int piece_moved, int piece_taken, int promotion_piece);
    static bool greater(const Move& move1, const Move& move2) { return move1.eval == move2.eval ? move1.depth < move2.depth : move1.eval > move2.eval; }
    static bool lesser(const Move& move1, const Move& move2) { return move1.eval == move2.eval ? move1.depth > move2.depth : move1.eval < move2.eval; }
    static bool isCapture(const Move& move) { return move.pieceTaken != -1 || move.promotionPiece != -1; }
    static bool isNotCapture(const Move& move) { return move.pieceTaken == -1 && move.promotionPiece == -1; }

    string getName();
};
