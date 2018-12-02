#include "move.h"
#include "board_definitions.h"
#include "bitops.h"
#include <math.h>


Move::Move(int from_square, int to_square, int piece_moved, int piece_taken, int promotion_piece)
{
    fromSquare = from_square;
    toSquare = to_square;
    pieceMoved = piece_moved;
    pieceTaken = piece_taken;
    promotionPiece = promotion_piece;
}

/*
getName - return the move notation in the form e2e4
*/
string Move::getName()
{
    string name = "";

    int from_row = fromSquare / 8;
    int from_col = fromSquare % 8;

    int to_row = toSquare / 8;
    int to_col = toSquare % 8;

    name += 'a' + from_col;
    name += '1' + from_row;

    name += 'a' + to_col;
    name += '1' + to_row;

    if (promotionPiece != -1)
        name += PIECE_CHARS[promotionPiece % 6 + 6];

    return name;
}


