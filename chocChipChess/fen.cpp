#include <iostream>
#include <string>
#include <sstream>

#include "position.h"

using namespace std;

void Position::loadFEN(string position)
{
    istringstream iss(position);
    string pos, side, castling, en, half_move, full_move;

    if (!(getline(iss, pos, ' ') && getline(iss, side, ' ') && getline(iss, castling, ' ') && getline(iss, en, ' ') && getline(iss, half_move, ' ') && getline(iss, full_move, ' ')))
        throw new invalid_argument("Invalid FEN (didn't find enough words)");

    for (uint8_t i = 0; i < BIT_BOARD_COUNT; i++)
        bitBoards[i] = 0;

    for (uint8_t board_pos = 0, fen_pos = 0; pos[fen_pos] != '\0'; fen_pos++) {

        char current_char = position[fen_pos];

        if (board_pos > 63)
            throw new invalid_argument("Invalid FEN (too many pieces)");

        const char *ptr = strchr(PIECE_CHARS, current_char);
        uint8_t index = (uint8_t)(ptr - PIECE_CHARS);

        if (ptr != NULL) {
            int row = 7 - board_pos / 8; // FEN starts from a8, goes left to right
            int col = board_pos % 8;
            bitBoards[index] |= 1ULL << (row * 8 + col);
            board_pos++;
        }
        else if ('9' > current_char && current_char > '0')
            board_pos += current_char - '0';
        else if (current_char != '/')
            throw new invalid_argument("Invalid FEN (unexpected character)");
    }

    updateSideBitBoards();

    Zobrist::init(4723456U);

    // side to move
    if (side == "w")
        sideToMove = WHITE;
    else if (side == "b")
        sideToMove = BLACK;
    else
        throw new invalid_argument("Invalid FEN, expected 'w' or 'b' for side to move");

    // castling
    castlingRights =
        (castling.find('K') != string::npos) |
        (castling.find('Q') != string::npos) << 1 |
        (castling.find('k') != string::npos) << 2 |
        (castling.find('q') != string::npos) << 3;

    updateCastlingRights();
    castlingRightsStack.empty();
    castlingRightsStack.push(castlingRights);

    // en passent
    enPassentStack.empty();
    enPassentStack.push(en == "-" ? NO_ENPASSENT : en[0] - 'a' + (en[1] - '1') * 8);

    halfMoveCount = stoi(half_move);

    zobristHash = calculateZobristHash();
    tt->clear();
    tt->setSize(ttSize);
}

string Position::toFEN()
{
    string pos = "";
    int board_pos = 0;
    int blank_spaces = 0;

    while (board_pos < 64) {
        int piece = 0;
        int row = board_pos / 8;
        int col = board_pos % 8;
        while (!(bitBoards[piece] & 1ULL << ((7 - row) * 8 + col))) {
            piece++;
            if (piece > 11) {
                blank_spaces++;
                break;
            }
        }

        if (piece != 12) {
            if (blank_spaces > 0) {
                pos += '0' + blank_spaces;
                blank_spaces = 0;
            }
            pos += PIECE_CHARS[piece];
        }

        if (board_pos % 8 == 7) {
            if (blank_spaces > 0) {
                pos += char('0' + blank_spaces);
                blank_spaces = 0;
            }
            if (board_pos < 63)
                pos += '/';
        }

        board_pos++;
    }

    string side, castling, en, half, full;
    side = sideToMove == WHITE ? 'w' : 'b';

    castling = "";
    if (castlingRights & 1) castling += "K";
    if (castlingRights & 2) castling += "Q";
    if (castlingRights & 4) castling += "k";
    if (castlingRights & 8) castling += "q";
    if (castling == "") castling = "-";

    if (enPassentStack.top() != NO_ENPASSENT) {
        char file = 'a' + enPassentStack.top() % 8;
        char col = '1' + enPassentStack.top() / 8;
        en = "xx";
        en[0] = file;
        en[1] = col;
    }
    else
        en = "-";

    half = '0';
    full = '1';

    return pos + " " + side + " " + castling + " " + en + " " + half + " " + full;
}
