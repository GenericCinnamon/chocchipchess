#include "position.h"
#include "board_definitions.h"
#include "bitops.h"


void Position::moveHopper(vector<Move>* move_list, int piece_type, const MoveDetails moves[], int arr_size) {
    u64 pieces = bitBoards[piece_type];
    u64 friendly_pieces = bitBoards[WHITE_PIECES + sideToMove];

    for (int i = 0; i < arr_size; i++) {
        const MoveDetails* this_move = &moves[i];

        u64 temp_board;
        temp_board = pieces;
        temp_board &= ~this_move->mask;
        shift_inplace(temp_board, this_move->shift_by);
        temp_board &= ~friendly_pieces;

        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            createMove(move_list, pos - this_move->shift_by, pos, piece_type);
        }
    }
}


void Position::moveSlider(vector<Move>*move_list, int piece_type, const MoveDetails moves[], int arr_size) {
    u64 pieces = bitBoards[piece_type];
    u64 friendly_pieces = bitBoards[WHITE_PIECES + sideToMove];
    u64 enemy_pieces = bitBoards[WHITE_PIECES + !sideToMove];

    for (int i = 0; i < arr_size; i++) {
        const MoveDetails* this_move = &moves[i];
        u64 temp_board = pieces;

        int counter = 1;
        while (temp_board) {
            temp_board &= ~this_move->mask;
            shift_inplace(temp_board, this_move->shift_by);
            temp_board &= ~friendly_pieces;

            u64 move_board = temp_board;
            while (move_board) {
                int pos = log2andclear_64(move_board);
                createMove(move_list, pos - this_move->shift_by * counter, pos, piece_type);
            }
            temp_board &= ~enemy_pieces;
            counter++;
        }
    }
}


void Position::moveWhitePawns(vector<Move>* move_list)
{
    // Pushes
    u64 temp_board, after_first_push, all_pieces;

    all_pieces = bitBoards[WHITE_PIECES] | bitBoards[BLACK_PIECES];
    temp_board = bitBoards[WHITE_PAWN];

    shift_north(temp_board);
    temp_board &= ~all_pieces;
    temp_board &= ~RANK8;
    after_first_push = temp_board;

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - NORTH, pos, WHITE_PAWN);
    }

    // Promotions
    temp_board = bitBoards[WHITE_PAWN];

    shift_north(temp_board);
    temp_board &= ~all_pieces;
    temp_board &= RANK8;

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - NORTH, pos, WHITE_PAWN, WHITE_ROOK);
        createMove(move_list, pos - NORTH, pos, WHITE_PAWN, WHITE_KNIGHT);
        createMove(move_list, pos - NORTH, pos, WHITE_PAWN, WHITE_BISHOP);
        createMove(move_list, pos - NORTH, pos, WHITE_PAWN, WHITE_QUEEN);
    }

    // Double push
    temp_board = after_first_push & RANK3;
    if (temp_board) {
        shift_north(temp_board);
        temp_board &= ~all_pieces;

        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            createMove(move_list, pos - NORTH - NORTH, pos, WHITE_PAWN);
        }
    }

    // North west attacks
    temp_board = bitBoards[WHITE_PAWN];
    temp_board &= ~FILEA; // Clear the A file so it does not wrap
    shift_north_west(temp_board);
    if (enPassentStack.top() != -1)
        temp_board &= bitBoards[BLACK_PIECES] | shift_by(1ULL, enPassentStack.top());
    else
        temp_board &= bitBoards[BLACK_PIECES];

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - NORTH_WEST, pos, WHITE_PAWN);
    }

    // North east attacks
    temp_board = bitBoards[WHITE_PAWN];
    temp_board &= ~FILEH; // Clear the H file so it does not wrap
    shift_north_east(temp_board);
    temp_board &= bitBoards[BLACK_PIECES] | shift_by(1ULL, enPassentStack.top());

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - NORTH_EAST, pos, WHITE_PAWN);
    }
}


void Position::moveBlackPawns(vector<Move>* move_list)
{
    // Pushes
    u64 temp_board, after_first_push, all_pieces;

    all_pieces = bitBoards[WHITE_PIECES] | bitBoards[BLACK_PIECES];
    temp_board = bitBoards[BLACK_PAWN];

    shift_south(temp_board);
    temp_board &= ~all_pieces;
    temp_board &= ~RANK1;
    after_first_push = temp_board;

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - SOUTH, pos, BLACK_PAWN);
    }

    // Promotions
    temp_board = bitBoards[BLACK_PAWN];

    shift_south(temp_board);
    temp_board &= ~all_pieces;
    temp_board &= RANK1;

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - SOUTH, pos, BLACK_PAWN, BLACK_ROOK);
        createMove(move_list, pos - SOUTH, pos, BLACK_PAWN, BLACK_KNIGHT);
        createMove(move_list, pos - SOUTH, pos, BLACK_PAWN, BLACK_BISHOP);
        createMove(move_list, pos - SOUTH, pos, BLACK_PAWN, BLACK_QUEEN);
    }

    // Double push
    temp_board = after_first_push & RANK6;
    if (temp_board) {
        shift_south(temp_board);
        temp_board &= ~all_pieces;

        while (temp_board) {
            int pos = log2andclear_64(temp_board);
            createMove(move_list, pos - SOUTH - SOUTH, pos, BLACK_PAWN);
        }
    }

    // North west attacks
    temp_board = bitBoards[BLACK_PAWN];
    temp_board &= ~FILEA; // Clear the A file so it does not wrap
    shift_south_west(temp_board);
    temp_board &= bitBoards[WHITE_PIECES] | shift_by(1ULL, enPassentStack.top());

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - SOUTH_WEST, pos, BLACK_PAWN);
    }

    // North east attacks
    temp_board = bitBoards[BLACK_PAWN];
    temp_board &= ~FILEH; // Clear the H file so it does not wrap
    shift_south_east(temp_board);
    temp_board &= bitBoards[WHITE_PIECES] | shift_by(1ULL, enPassentStack.top());

    while (temp_board) {
        int pos = log2andclear_64(temp_board);
        createMove(move_list, pos - SOUTH_EAST, pos, BLACK_PAWN);
    }
}


void Position::moveCastling(vector<Move>* move_list) {
    int square_offset = 7 * 8 * sideToMove;
    int friendly_king = WHITE_KING + 6 * sideToMove;

    if (!bitBoards[friendly_king])
        return;

    int friendly_king_pos = log2_64(bitBoards[friendly_king]);
    u64 q_castle_check = 0xEULL << square_offset;
    u64 q_castle_check_check = 0xCULL << square_offset;
    u64 k_castle_check = 0x60ULL << square_offset;
    u64 all_pieces = bitBoards[WHITE_PIECES] | bitBoards[BLACK_PIECES];



    if (castlingRights & (1 << (1 + 2 * sideToMove))) { // Queenside
        if (!(all_pieces & q_castle_check)) {
            bitBoards[friendly_king] |= q_castle_check_check;
            if (!checkCheck(sideToMove)) {
                bitBoards[friendly_king] = 1ULL << friendly_king_pos;
                createMove(move_list, E1 + square_offset, C1 + square_offset, friendly_king);
            }
            else
                bitBoards[friendly_king] = 1ULL << friendly_king_pos;
        }
    }
    
    if (castlingRights & (1 << (2 * sideToMove))) { // Kingside
        if (!(all_pieces & k_castle_check)) {
            bitBoards[friendly_king] |= k_castle_check;
            if (!checkCheck(sideToMove)) {
                bitBoards[friendly_king] = 1ULL << friendly_king_pos;
                createMove(move_list, E1 + square_offset, G1 + square_offset, friendly_king);
            }
            else
                bitBoards[friendly_king] = 1ULL << friendly_king_pos;
        }
    }
}


bool Position::checkCheck(bool side)
{
    int side_offset = 6 * side;
    u64 friendly_kings = bitBoards[WHITE_KING + side_offset];
    u64 friendly_pieces = bitBoards[WHITE_PIECES + side];
    u64 enemy_pieces = bitBoards[BLACK_PIECES - side];

    // Knights
    for (const auto i : KNIGHT_MOVES) {
        if (shift_by(friendly_kings & ~i.mask, i.shift_by) & bitBoards[BLACK_KNIGHT - side_offset])
            return true;
    }

    // Pawn north/south west attacks
    if (shift_by(~FILEA & bitBoards[BLACK_PAWN - side_offset], WEST + (side == WHITE ? SOUTH : NORTH)) & friendly_kings)
        return true;

    // Pawn north/south east attacks
    if (shift_by(~FILEH & bitBoards[BLACK_PAWN - side_offset], EAST + (side == WHITE ? SOUTH : NORTH)) & friendly_kings)
        return true;

    // Kings
    for (const auto i : KING_MOVES) {
        if (shift_by(friendly_kings & ~i.mask, i.shift_by) & bitBoards[BLACK_KING - side_offset])
            return true;
    }

    // Bishops and Queens
    for (const auto& i : BISHOP_MOVES) {
        u64 temp_board = shift_by(friendly_kings & ~i.mask, i.shift_by);

        while (temp_board) {
            temp_board &= ~friendly_pieces;

            if (temp_board & (bitBoards[BLACK_BISHOP - side_offset] | bitBoards[BLACK_QUEEN - side_offset]))
                return true;

            temp_board &= ~enemy_pieces;
            temp_board &= ~i.mask;
            shift_inplace(temp_board, i.shift_by);
        }
    }

    //Rooks and Queens
    for (const auto& i : ROOK_MOVES) {
        u64 temp_board = shift_by(friendly_kings & ~i.mask, i.shift_by);

        while (temp_board) {
            temp_board &= ~friendly_pieces;

            if (temp_board & (bitBoards[BLACK_ROOK - side_offset] | bitBoards[BLACK_QUEEN - side_offset]))
                return true;

            temp_board &= ~enemy_pieces;
            temp_board &= ~i.mask;
            shift_inplace(temp_board, i.shift_by);
        }
    }

    return false;
}


void Position::updateCastlingRights() {
    if (!(bitBoards[WHITE_KING] & (0x1ULL << E1)))
        castlingRights &= ~WHITE_CASTLING;
    else {
        if (!(bitBoards[WHITE_ROOK] & (0x1ULL << H1)))
            castlingRights &= ~WHITE_KINGSIDE;
        if (!(bitBoards[WHITE_ROOK] & (0x1ULL << A1)))
            castlingRights &= ~WHITE_QUEENSIDE;
    }

    if (!(bitBoards[BLACK_KING] & (0x1ULL << E8)))
        castlingRights &= ~BLACK_CASTLING;
    else {
        if (!(bitBoards[BLACK_ROOK] & (0x1ULL << H8)))
            castlingRights &= ~BLACK_KINGSIDE;
        if (!(bitBoards[BLACK_ROOK] & (0x1ULL << A8)))
            castlingRights &= ~BLACK_QUEENSIDE;
    }
}
