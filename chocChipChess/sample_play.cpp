#include "sample_play.h"

#include <iostream>
#include <string>
#include "position.h"

using namespace std;

SamplePlay::SamplePlay()
{
    Position* pos = new Position(STARTPOS_FEN);
}

bool SamplePlay::processLine(string input) {
    /*
    'move' - make a move with depth 2
    */
    if (input == "move" || input == " ") {
        auto move = pos->makeMove(2);
        cout << pos->printBoard() << endl;
        // If there aren't any moves left just quit
        if (move == NULL)
            return true;
        cout << "I moved " << move->getName() << endl;
    }
    /*
    'quit'
    */
    else if (input == "q" || input == "quit")
        return false;
    /*
    'auto' - play out 20 moves
    */
    else if (input == "auto") {
        for (int i = 0; i < 20; i++) {
            auto move = pos->makeMove(6);
            cout << pos->printBoard() << endl;
            if (move == NULL)
                return false;
            cout << "I moved " << move->getName() << endl;
        }
    }
    /*
    'print' - show the board
    */
    else if (input == "d" || input == "?" || input == "print" || input == "p")
        cout << pos->printBoard();
    /*
    'fen' - print FEN for the current position
    */
    else if (input == "fen") {
        cout << pos->toFEN() << endl;
    }
    /*
    'load' - load fen from a string
    */
    else if (input == "load") {
        cout << "input FEN: " << endl;
        getline(cin, input);
        pos->loadFEN(input);
    }
    /*
    Treat the input as a move
    */
    else {
        if (!pos->inputMove(input.c_str()))
            printf("No move with the name %s\n", input.c_str());
        else
            cout << pos->printBoard();
    }

    return false;
}