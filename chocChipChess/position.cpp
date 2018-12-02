#include <vector>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <exception>

#include "move.h"
#include "eval.h"
#include "bitops.h"
#include "position.h"
#include "board_definitions.h"
#include "zobrist.h"
#include "limits.h"
#include "transposition_table.h"

using namespace std;

/*
Position - Create the given position
*/
Position::Position(string position) {
    tt = new TranspositionTable(1000);
    loadFEN(position);
    output = new OutputHelper();
}

/*
Position - Create the given position using a shared move table
*/
Position::Position(string position, TranspositionTable* shared_tt) {
    delete tt;
    tt = shared_tt;
    loadFEN(position);
    output = new OutputHelper();
}

/*
updateSideBitBoards - Recompute the bitboard for showing which squares are occupied by which colour
*/
void Position::updateSideBitBoards()
{
    bitBoards[WHITE_PIECES] = 0ULL;
    bitBoards[BLACK_PIECES] = 0ULL;
    for (uint8_t i = WHITE_PAWN; i < BLACK_PAWN; i++) {
        bitBoards[WHITE_PIECES] |= bitBoards[i];
        bitBoards[BLACK_PIECES] |= bitBoards[i+6];
    }
}

/*
genMoves - Return a list of all possible moves in the current position
*/
vector<Move> Position::genMoves()
{
    vector<Move> move_list;
    
    int side_offset = 6 * sideToMove;

    // Find knight moves
    moveHopper(&move_list, WHITE_KNIGHT + side_offset, KNIGHT_MOVES, 8);
    // Find king moves
    moveHopper(&move_list, WHITE_KING + side_offset, KING_MOVES, 8);
    // Find rook moves
    moveSlider(&move_list, WHITE_ROOK + side_offset, ROOK_MOVES, 4);
    // Find bishop moves
    moveSlider(&move_list, WHITE_BISHOP + side_offset, BISHOP_MOVES, 4);
    // Find queen moves
    moveSlider(&move_list, WHITE_QUEEN + side_offset, QUEEN_MOVES, 8);
    // Move pawns - The moves are not symmetrical between colours and so are dealt with separately
    sideToMove == WHITE ? moveWhitePawns(&move_list) : moveBlackPawns(&move_list);
    // Castling
    moveCastling(&move_list);

    return move_list;
}

/*
inputMove - play the given move on the board
*/
bool Position::inputMove(string move_name)
{
    auto move_list = genMoves();
    string possible_moves = "";
    for (auto move : move_list) {
        string name = move.getName();
        if (name == move_name) {
            performMove(&move);
            return true;
        }
        possible_moves += name + " ";
    }

    //output->output("info debug possible moves found " + possible_moves);

    return false;
}

/*
makeMove - make the best move after searching a given depth
*/
Move * Position::makeMove(int depth)
{

    for (int i = 0; i < depth; i++)
        //alphabeta(i, INT_MIN, INT_MAX);
        negamax(i + 1, INT_MIN, INT_MAX);
    return makeMove();
}

/*
makeMove - make the best move of depth 0
*/
Move* Position::makeMove()
{
    Move* move = topMove();
    for (auto move : genMoves())
        output->debug("info debug move " + move.getName() + " eval cp " + to_string(move.eval));
    performMove(move);

    return move;
}


/*
printBoard - return an ascii display of the board
*/
string Position::printBoard()
{
    string result = "";
    char board_characters[65];

    // Set everything to spaces and null terminate the string
    for (int i = 0; i < 64; i++)
        board_characters[i] = ' ';

    // Place all the piece characters into a single array
    board_characters[64] = NULL;

    for (uint8_t p = 0; p < 12; p++) {
        uint64_t temp_board = bitBoards[p];
        while (temp_board) {
            uint8_t pos = log2andclear_64(temp_board);
            board_characters[pos] = PIECE_CHARS[p];
        }
    }

    // Top bar
    result += " |________________________________|\n";

    // For each row
    for (int y = 7; y >-1; y--) {
        // Rank characters
        result += to_string(y+1) + "|";

        // For each column
        for (int x = 0; x < 8; x++) {
            result += " ";
            result += board_characters[x + y * 8];
            result += " ";
            result += ":";

        }
        result += "|\n";
        result += " |--------------------------------|\n";
    }

    // File characters
    result += "  a   b   c   d   e   f   g   h\n";
    return result;
}

/*
printBitBoards - debug - display all bit boards which make up this position
*/
string Position::printBitBoards()
{
    string result = "";
    for (int p = 0; p < BIT_BOARD_COUNT; p++) {
        result += to_string(p);
        result += debugPrintBitBoard(bitBoards[p]);
    }
    return result;
}

/*
directOutput - update the output destination
*/
void Position::directOutput(OutputHelper* helper)
{
    if (output != NULL)
        delete output;
    output = helper;
}

/*
createMove - add a move to the move list
*/
void Position::createMove(vector<Move>* move_list, int from_square, int to_square, int piece) {
    createMove(move_list, from_square, to_square, piece, -1);
}

/*
createMove - add a move to the move list and allow for promotion
*/
void Position::createMove(vector<Move>* move_list, int from_square, int to_square, int piece, int promotion_piece) {
    // Check if this is an attack
    u64 target_board = 1ULL << to_square;
    int piece_taken = -1;
    
    if (bitBoards[BLACK_PIECES - sideToMove] & target_board)
        for (int i = 6 * (1 - sideToMove); i < 6 * (2 - sideToMove); i++)
            if (bitBoards[i] & target_board) {
                piece_taken = i;
                break;
            }

    Move move(from_square, to_square, piece, piece_taken, promotion_piece);

    performMove(&move);
    if (!checkCheck(!sideToMove)) {
        auto entry = tt->retrieve(zobristHash);
        move.eval = entry == NULL ? calculateStaticEval() : entry->eval;
        move.depth = entry == NULL ? 0 : entry->depth;
        move_list->push_back(move);
    }
    undoMove(&move);
}

/*
performMove - perform the given move on the board
*/
void Position::performMove(Move* move)
{

    // Clear from square
    clear_bit(bitBoards[move->pieceMoved], move->fromSquare);
    clear_bit(bitBoards[WHITE_PIECES + sideToMove], move->fromSquare);
    zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->fromSquare];
    
    // Set to square
    set_bit(bitBoards[move->pieceMoved], move->toSquare);
    set_bit(bitBoards[WHITE_PIECES + sideToMove], move->toSquare);
    zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->toSquare];


    // Clear any captured piece
    if (move->pieceTaken != -1) {
        clear_bit(bitBoards[move->pieceTaken], move->toSquare);
        clear_bit(bitBoards[BLACK_PIECES - sideToMove], move->toSquare);
        zobristHash ^= Zobrist::PIECE_INTS[move->pieceTaken][move->toSquare];
    }

    // En passent
    int side_offset = sideToMove * 6;
    if (move->pieceMoved == WHITE_PAWN + side_offset && move->toSquare == enPassentStack.top()) {
        int pawn_square = sideToMove == WHITE ? move->toSquare - 8 : move->toSquare + 8;
        clear_bit(bitBoards[BLACK_PAWN - side_offset], pawn_square);
        clear_bit(bitBoards[BLACK_PIECES - sideToMove], pawn_square);
        zobristHash ^= Zobrist::PIECE_INTS[BLACK_PAWN - side_offset][pawn_square];
    }

    // Promotion
    if (move->promotionPiece != -1) {
        clear_bit(bitBoards[move->pieceMoved], move->toSquare);
        zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->toSquare];
        set_bit(bitBoards[move->promotionPiece], move->toSquare);
        zobristHash ^= Zobrist::PIECE_INTS[move->promotionPiece][move->toSquare];
    }

    // Move rook if there was a castle
    int square_offset = sideToMove * 8 * 7;
    if (move->pieceMoved == WHITE_KING + side_offset && abs(move->fromSquare - move->toSquare) == 2) {
        if (move->toSquare % 8 == C1) { // queenside
            clear_bit(bitBoards[WHITE_ROOK + side_offset], A1 + square_offset);
            clear_bit(bitBoards[WHITE_PIECES + sideToMove], A1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][A1 + square_offset];
            set_bit(bitBoards[WHITE_ROOK + side_offset], D1 + square_offset);
            set_bit(bitBoards[WHITE_PIECES + sideToMove], D1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][D1 + square_offset];
        }
        else if (move->toSquare % 8 == G1) {
            clear_bit(bitBoards[WHITE_ROOK + side_offset], H1 + square_offset);
            clear_bit(bitBoards[WHITE_PIECES + sideToMove], H1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][H1 + square_offset];
            set_bit(bitBoards[WHITE_ROOK + side_offset], F1 + square_offset);
            set_bit(bitBoards[WHITE_PIECES + sideToMove], F1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][F1 + square_offset];
        }
        else {
            throw new logic_error("bad castle");
        }
    }

    // Castling rights
    char previous_castling_rights = castlingRights;
    updateCastlingRights();
    castlingRightsStack.push(castlingRights);

    // Castling rights hash
    if (WHITE_KINGSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[0];
    if (WHITE_QUEENSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[1];
    if (BLACK_KINGSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[2];
    if (BLACK_QUEENSIDE & (castlingRights ^  previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[3];
    
    // Enpassent hash
    zobristHash ^= (enPassentStack.top() != NO_ENPASSENT) * Zobrist::EN_PASSENT_INTS[enPassentStack.top() % 8];
    if (move->pieceMoved == WHITE_PAWN + side_offset && abs(move->toSquare - move->fromSquare) == 16)
        enPassentStack.push(sideToMove == WHITE ? move->toSquare - 8 : move->toSquare + 8);
    else
        enPassentStack.push(NO_ENPASSENT);
    zobristHash ^= (enPassentStack.top() != NO_ENPASSENT) * Zobrist::EN_PASSENT_INTS[enPassentStack.top() % 8];

    // Update hash and side
    zobristHash ^= Zobrist::SIDE_IS_BLACK_INT;
    sideToMove = !sideToMove;
    halfMoveCount++;
}


void Position::undoMove(Move* move)
{
    // Update side and hash
    zobristHash ^= Zobrist::SIDE_IS_BLACK_INT;
    sideToMove = !sideToMove;
    halfMoveCount--;
    zobristHash ^= (enPassentStack.top() != NO_ENPASSENT) * Zobrist::EN_PASSENT_INTS[enPassentStack.top() % 8];
    enPassentStack.pop();
    zobristHash ^= (enPassentStack.top() != NO_ENPASSENT) * Zobrist::EN_PASSENT_INTS[enPassentStack.top() % 8];

    // Clear to square
    clear_bit(bitBoards[move->pieceMoved], move->toSquare);
    clear_bit(bitBoards[WHITE_PIECES + sideToMove], move->toSquare);
    zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->toSquare];

    // Set from square
    set_bit(bitBoards[move->pieceMoved], move->fromSquare);
    set_bit(bitBoards[WHITE_PIECES + sideToMove], move->fromSquare);
    zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->fromSquare];

    // Undo any capture
    if (move->pieceTaken != -1) {
        set_bit(bitBoards[move->pieceTaken], move->toSquare);
        set_bit(bitBoards[BLACK_PIECES - sideToMove], move->toSquare);
        zobristHash ^= Zobrist::PIECE_INTS[move->pieceTaken][move->toSquare];
    }

    // Undo any enpassent
    int side_offset = sideToMove * 6;
    if (move->pieceMoved == WHITE_PAWN + side_offset && move->toSquare == enPassentStack.top()) {
        int pawn_square = sideToMove == WHITE ? move->toSquare - 8 : move->toSquare + 8;
        set_bit(bitBoards[BLACK_PAWN - side_offset], pawn_square);
        set_bit(bitBoards[BLACK_PIECES - sideToMove], pawn_square);
        zobristHash ^= Zobrist::PIECE_INTS[BLACK_PAWN - side_offset][pawn_square];
    }

    // Promotion
    if (move->promotionPiece != -1) {
        clear_bit(bitBoards[move->promotionPiece], move->toSquare);
        zobristHash ^= Zobrist::PIECE_INTS[move->promotionPiece][move->toSquare];
        //set_bit(bitBoards[move->pieceMoved], move->fromSquare);
        //zobristHash ^= Zobrist::PIECE_INTS[move->pieceMoved][move->toSquare];
    }

    // Castling
    int square_offset = sideToMove * 8 * 7;
    if (move->pieceMoved == WHITE_KING + side_offset && abs(move->fromSquare - move->toSquare) == 2) {
        if (move->toSquare % 8 == C1) { // queenside
            clear_bit(bitBoards[WHITE_ROOK + side_offset], D1 + square_offset);
            clear_bit(bitBoards[WHITE_PIECES + sideToMove], D1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][D1 + square_offset];
            set_bit(bitBoards[WHITE_ROOK + side_offset], A1 + square_offset);
            set_bit(bitBoards[WHITE_PIECES + sideToMove], A1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][A1 + square_offset];
        }
        else if (move->toSquare % 8 == G1) {
            clear_bit(bitBoards[WHITE_ROOK + side_offset], F1 + square_offset);
            clear_bit(bitBoards[WHITE_PIECES + sideToMove], F1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][F1 + square_offset];
            set_bit(bitBoards[WHITE_ROOK + side_offset], H1 + square_offset);
            set_bit(bitBoards[WHITE_PIECES + sideToMove], H1 + square_offset);
            zobristHash ^= Zobrist::PIECE_INTS[WHITE_ROOK + side_offset][H1 + square_offset];
        }
        else {
            throw new logic_error("bad castle");
        }
    }

    char previous_castling_rights = castlingRights;
    castlingRightsStack.pop();
    castlingRights = castlingRightsStack.top();

    // Castling rights hash
    if (WHITE_KINGSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[0];
    if (WHITE_QUEENSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[1];
    if (BLACK_KINGSIDE & (castlingRights ^ previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[2];
    if (BLACK_QUEENSIDE & (castlingRights ^  previous_castling_rights))
        zobristHash ^= Zobrist::CASTLING_INTS[3];

}

/*
calculateStaticEval - basic evalution of a position based on piece value and location
*/
int Position::calculateStaticEval() // TODO optimise
{
    int eval = 0;

    u64 temp_board;
    for (int i = 0; i < 6; i++) {
        temp_board = bitBoards[i];
        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            eval += pieceWorth[i][63 - pos];
        }
    }

    for (int i = 6; i < 12; i++) {
        temp_board = bitBoards[i];
        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            eval -= pieceWorth[i - 6][pos];
        }
    }

    return eval;
}






u64 Position::calculateZobristHash()
{
    u64 zobrist = 0;

    u64 temp_board;
    for (int i = 0; i < 12; i++) {
        temp_board = bitBoards[i];
        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            zobrist ^= Zobrist::PIECE_INTS[i][pos];
        }
    }

    if (castlingRights & WHITE_KINGSIDE)    zobrist ^= Zobrist::CASTLING_INTS[0];
    if (castlingRights & WHITE_QUEENSIDE)    zobrist ^= Zobrist::CASTLING_INTS[1];
    if (castlingRights & BLACK_KINGSIDE)    zobrist ^= Zobrist::CASTLING_INTS[2];
    if (castlingRights & BLACK_QUEENSIDE)    zobrist ^= Zobrist::CASTLING_INTS[3];

    zobrist ^= (sideToMove == BLACK) * Zobrist::SIDE_IS_BLACK_INT;
    zobrist ^= (enPassentStack.top() != NO_ENPASSENT) * Zobrist::EN_PASSENT_INTS[enPassentStack.top() % 8];
    
    return zobrist;
}

