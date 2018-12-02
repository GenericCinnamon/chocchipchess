#include "gtest/gtest.h"
#include "PositionExtras.h"

TEST(positionClassTests, LoadFEN) {
    
    PositionExtras test_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Output FEN
    EXPECT_EQ(test_board.toFEN(), "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Pawns
    EXPECT_EQ(test_board.bitBoards[WHITE_PAWN], (uint64_t)0xff00);
    EXPECT_EQ(test_board.bitBoards[BLACK_PAWN], (uint64_t)0xff000000000000);

    // Rooks
    EXPECT_EQ(test_board.bitBoards[WHITE_ROOK], (uint64_t)0x81);
    EXPECT_EQ(test_board.bitBoards[BLACK_ROOK], (uint64_t)0x8100000000000000);

    // Knights
    EXPECT_EQ(test_board.bitBoards[WHITE_KNIGHT], (uint64_t)0x42);
    EXPECT_EQ(test_board.bitBoards[BLACK_KNIGHT], (uint64_t)0x4200000000000000);

    // Bishops
    EXPECT_EQ(test_board.bitBoards[WHITE_BISHOP], (uint64_t)0x24);
    EXPECT_EQ(test_board.bitBoards[BLACK_BISHOP], (uint64_t)0x2400000000000000);

    // Queens
    EXPECT_EQ(test_board.bitBoards[WHITE_QUEEN], (uint64_t)0x8);
    EXPECT_EQ(test_board.bitBoards[BLACK_QUEEN], (uint64_t)0x800000000000000);

    // Kings
    EXPECT_EQ(test_board.bitBoards[WHITE_KING], (uint64_t)0x10);
    EXPECT_EQ(test_board.bitBoards[BLACK_KING], (uint64_t)0x1000000000000000);

    // Colors
    EXPECT_EQ(test_board.bitBoards[WHITE_PIECES], (uint64_t)0xffff);
    EXPECT_EQ(test_board.bitBoards[BLACK_PIECES], (uint64_t)0xffff000000000000);

    EXPECT_NO_THROW(test_board.throwIfInvalid());
}


TEST(PositionClassTests, PawnMoves) {
    PositionExtras test_board("8/4p3/8/8/8/8/4P3/8 w - - 0 1");
    EXPECT_EQ(test_board.perft(2), 4);
    EXPECT_NO_THROW(test_board.throwIfInvalid());
}


TEST(PositionClassTests, RookMoves) {
    PositionExtras test_board("8/8/8/4R3/8/8/8/8 w - - 0 1");
    EXPECT_EQ(test_board.perft(1), 14);
    EXPECT_NO_THROW(test_board.throwIfInvalid());

    PositionExtras test_board2("8/8/8/4r3/8/8/8/8 b - - 0 1");
    EXPECT_EQ(test_board2.perft(1), 14);
    EXPECT_NO_THROW(test_board2.throwIfInvalid());
}


TEST(PositionClassTests, BishopMoves) {
    PositionExtras test_board("8/8/8/3Bb3/8/8/8/8 w KQkq - 0 1");
    EXPECT_EQ(test_board.perft(2), 169);
    EXPECT_NO_THROW(test_board.throwIfInvalid());
}


TEST(PositionClassTests, KingMoves) {
    PositionExtras test_board("8/4K3/8/8/8/8/4k3/8 w - - 0 1");
    EXPECT_EQ(test_board.perft(2), 64);
    EXPECT_NO_THROW(test_board.throwIfInvalid());
}


TEST(PositionClassTests, QueenMoves) {
    PositionExtras test_board("8/8/8/4Q3/8/8/8/8 w - - 0 1");
    EXPECT_EQ(test_board.perft(1), 27);
    EXPECT_NO_THROW(test_board.throwIfInvalid());

    PositionExtras test_board2("8/8/8/4q3/8/8/8/8 b - - 0 1");
    EXPECT_EQ(test_board2.perft(1), 27);
    EXPECT_NO_THROW(test_board2.throwIfInvalid());
}


TEST(PositionClassTests, Castling) {
    // Kingside white
    PositionExtras test_board = PositionExtras("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 6 5");
    EXPECT_EQ(test_board.genMoves().size(), 25);
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    EXPECT_TRUE(test_board.inputMove("e1g1"));

    // Kingside black
    PositionExtras test_board2 = PositionExtras("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 6 5");
    EXPECT_EQ(test_board2.genMoves().size(), 25);
    EXPECT_NO_THROW(test_board2.throwIfInvalid());
    EXPECT_TRUE(test_board.inputMove("e8g8"));

    // Queenside white
    PositionExtras test_board3 = PositionExtras("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 6 5");
    EXPECT_EQ(test_board3.genMoves().size(), 25);
    EXPECT_NO_THROW(test_board3.throwIfInvalid());
    EXPECT_TRUE(test_board3.inputMove("e1c1"));

    // Queenside black
    PositionExtras test_board4 = PositionExtras("r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 6 5");
    EXPECT_EQ(test_board4.genMoves().size(), 25);
    EXPECT_NO_THROW(test_board4.throwIfInvalid());
    EXPECT_TRUE(test_board4.inputMove("e8c8"));

    // Validity checks
    unordered_map<uint64_t, string> tt;
    EXPECT_NO_THROW(test_board.perftWithTT(1, tt));
    EXPECT_NO_THROW(test_board2.perftWithTT(1, tt));
    EXPECT_NO_THROW(test_board3.perftWithTT(1, tt));
    EXPECT_NO_THROW(test_board4.perftWithTT(1, tt));

}


TEST(PositionClassTests, Perft) {
    PositionExtras test_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");

    // Perft
    EXPECT_EQ(test_board.perft(1), 20);
    EXPECT_EQ(test_board.perft(2), 400);
    EXPECT_EQ(test_board.perft(3), 8902);
    EXPECT_EQ(test_board.perft(4), 197281);
    //EXPECT_EQ(test_board.perft(5), 4865609);

    PositionExtras test_board2("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}


TEST(CalculationTests, MateInOne) {
    PositionExtras test_board("k7/8/8/8/8/1r6/r7/7K b - - 0 1");
    EXPECT_EQ(test_board.makeMove(3)->getName(), "b3b1");

    EXPECT_NO_THROW(test_board.throwIfInvalid());

}

TEST(CalculationTests, MateInTwo) {
    PositionExtras test_board("k7/8/8/8/r7/1r6/8/7K b - - 0 1");

    string best_move = test_board.makeMove(3)->getName();
    EXPECT_TRUE(best_move == "a4a2" || best_move == "b3b2");

    EXPECT_NO_THROW(test_board.throwIfInvalid());
}

TEST(CalculationTests, MateInThree) {
    PositionExtras test_board("k7/8/8/r7/1r6/8/7K/8 w - - 0 1");

    for (int i = 0; i < 6; i++)
        test_board.makeMove(7);
    
    EXPECT_TRUE(test_board.genMoves().size() == 0);

    EXPECT_NO_THROW(test_board.throwIfInvalid());
}

TEST(PositionClassTests, CheckCheck) {
    EXPECT_TRUE(PositionExtras("K7/8/8/8/8/8/7Q/7k b - - 0 1").checkCheck(BLACK));
    EXPECT_TRUE(PositionExtras("2k5/8/8/8/8/8/2p5/3K4 w - - 0 1").checkCheck(WHITE));
    EXPECT_TRUE(PositionExtras("2k5/8/8/8/2R5/8/8/3K4 b - - 0 1").checkCheck(BLACK));
    EXPECT_TRUE(PositionExtras("rnb1kbnr/pppp1ppp/8/4p3/7q/8/PPPPP1PP/RNBQKBNR w KQkq - 0 1").checkCheck(WHITE));
    EXPECT_FALSE(PositionExtras("rnb1kbnr/pppp1ppp/8/4p3/7q/5PP1/PPPPP2P/RNBQKBNR w KQkq - 0 1").checkCheck(WHITE));
    EXPECT_TRUE(PositionExtras("K7/8/8/8/8/1R6/8/R5k1 b - - 0 1").checkCheck(BLACK));
    EXPECT_TRUE(PositionExtras("K7/8/8/8/8/1R6/8/R5k1 w - - 0 1").checkCheck(BLACK));
    EXPECT_FALSE(PositionExtras("k7/8/8/8/8/1r6/r7/7K b - - 0 1").checkCheck(WHITE));


    EXPECT_FALSE(PositionExtras("4k2r/3b1ppp/1R6/p2p4/5nP1/4KP1B/P6P/6NR b kq - 1 1").checkCheck(WHITE));
    EXPECT_FALSE(PositionExtras("4k3/8/1R6/3p4/5nP1/4KP1B/P6P/6NR b - - 1 1").checkCheck(WHITE));
    EXPECT_FALSE(PositionExtras("4k3/8/8/8/5n2/4K3/8/8 b - - 1 1").checkCheck(WHITE));
}

TEST(PositionClassTests, GetOutOfCheck1) {
    PositionExtras test_board("rnb1k1nr/pppp1ppp/8/6q1/1bB5/4PP2/PPPP1P1P/RNBQ1RK1 w kq - 1 6");
    EXPECT_EQ(test_board.genMoves().size(), 1);
    EXPECT_NO_THROW(test_board.throwIfInvalid());
}

TEST(PositionClassTests, BadMove1) {
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("e2e4"));
    EXPECT_TRUE(test_board.inputMove("e7e5"));
    EXPECT_TRUE(test_board.inputMove("g1f3"));
    EXPECT_TRUE(test_board.inputMove("f7f5"));
    EXPECT_TRUE(test_board.inputMove("e4f5"));
    EXPECT_TRUE(test_board.inputMove("g8e7"));
    EXPECT_TRUE(test_board.inputMove("f3e5"));

    EXPECT_FALSE(test_board.inputMove("f8f5"));

    EXPECT_NO_THROW(test_board.throwIfInvalid());
}

TEST(PositionClassTests, BadMove2) {
    //position startpos moves e2e4 e7e5 g1f3 f7f5 f1d3 g8e7 e1g1
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("e2e4"));
    EXPECT_TRUE(test_board.inputMove("e7e5"));
    EXPECT_TRUE(test_board.inputMove("g1f3"));
    EXPECT_TRUE(test_board.inputMove("f7f5"));
    EXPECT_TRUE(test_board.inputMove("f1d3"));
    EXPECT_TRUE(test_board.inputMove("g8e7"));
    EXPECT_TRUE(test_board.inputMove("e1g1"));

    EXPECT_FALSE(test_board.inputMove("f8f6"));

    EXPECT_NO_THROW(test_board.throwIfInvalid());
}

TEST(PositionClassTests, BadMove3) {
    //position startpos moves e2e4 e7e5 g1f3 b8c6 f1d3 c6d4 e1g1 d4f3 d1f3 c7c6 d3c4 d7d5 e4d5 b7b5 c4b3 c6d5 f1d1 c8e6 d2d4 e8e7 d4e5 f7f5 b3d5 d8a5 d5e6 a5e1 d1e1 a8b8 f3c6
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("e2e4"));
    EXPECT_TRUE(test_board.inputMove("e7e5"));
    EXPECT_TRUE(test_board.inputMove("g1f3"));
    EXPECT_TRUE(test_board.inputMove("b8c6"));
    EXPECT_TRUE(test_board.inputMove("f1d3"));
    EXPECT_TRUE(test_board.inputMove("c6d4"));
    EXPECT_TRUE(test_board.inputMove("e1g1"));
    EXPECT_TRUE(test_board.inputMove("d4f3"));
    EXPECT_TRUE(test_board.inputMove("d1f3"));
    EXPECT_TRUE(test_board.inputMove("c7c6"));
    EXPECT_TRUE(test_board.inputMove("d3c4"));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    EXPECT_TRUE(test_board.inputMove("d7d5"));
    EXPECT_TRUE(test_board.inputMove("e4d5"));
    EXPECT_TRUE(test_board.inputMove("b7b5"));
    EXPECT_TRUE(test_board.inputMove("c4b3"));
    EXPECT_TRUE(test_board.inputMove("c6d5"));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    EXPECT_TRUE(test_board.inputMove("f1d1"));
    EXPECT_TRUE(test_board.inputMove("c8e6"));
    EXPECT_TRUE(test_board.inputMove("d2d4"));
    EXPECT_TRUE(test_board.inputMove("e8e7"));
    EXPECT_TRUE(test_board.inputMove("d4e5"));
    EXPECT_TRUE(test_board.inputMove("f7f5"));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    EXPECT_TRUE(test_board.inputMove("b3d5"));
    EXPECT_TRUE(test_board.inputMove("d8a5"));
    EXPECT_TRUE(test_board.inputMove("d5e6"));
    EXPECT_TRUE(test_board.inputMove("a5e1"));
    EXPECT_TRUE(test_board.inputMove("d1e1"));
    EXPECT_TRUE(test_board.inputMove("a8b8"));
    EXPECT_TRUE(test_board.inputMove("f3c6"));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    unordered_map<uint64_t, string> tt;
    EXPECT_NO_THROW(test_board.perftWithTT(3, tt));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
    EXPECT_FALSE(test_board.inputMove("f8d8"));
    EXPECT_NO_THROW(test_board.throwIfInvalid());
}


TEST(MovementTests, FEN2) {
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("e2e4"));
    EXPECT_EQ(test_board.toFEN(), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
}


TEST(MovementTests, EnPassent1) {
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("e2e4"));
    EXPECT_TRUE(test_board.inputMove("h7h6"));
    EXPECT_TRUE(test_board.inputMove("e4e5"));
    EXPECT_TRUE(test_board.inputMove("f7f5"));
    EXPECT_TRUE(test_board.inputMove("e5f6"));
    EXPECT_NO_THROW(test_board.perftWithTT(2, unordered_map<uint64_t, string>()));
}


TEST(MovementTests, BadMove4) {
    PositionExtras test_board(STARTPOS_FEN);
    EXPECT_TRUE(test_board.inputMove("d2d4"));
    EXPECT_TRUE(test_board.inputMove("d7d5"));
    EXPECT_TRUE(test_board.inputMove("b1c3"));
    EXPECT_TRUE(test_board.inputMove("f7f6"));
    EXPECT_TRUE(test_board.inputMove("c1f4"));
    EXPECT_TRUE(test_board.inputMove("g7g5"));
    EXPECT_TRUE(test_board.inputMove("f4g3"));
    EXPECT_NO_THROW(test_board.perftWithTT(3, unordered_map<uint64_t, string>()));
}

