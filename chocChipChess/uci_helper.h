#pragma once
#include <string>
#include <thread>
#include "output_helper.h"
#include "position.h"
#include "transposition_table.h"
#include "input_handler.h"
#define AVERAGE_GAME_LENGTH 30

using namespace std;


class UCIHelper : public InputHandler {
private:
    OutputHelper output;
    /* Store the current board position */
    Position* pos;
    /* Board to play ponder moves out */
    Position* ponderBoard;
    /* Thread to perform moves on the ponder board */
    thread* ponderThread;
    /* Transposition table to persist knowledge on */
    TranspositionTable sharedTT;

    void initNewGame();
    void loadPosition(string fen);
    void positionCommand(string line);
    void stopPonder();
    void sendToGUI(string line);
public:
    UCIHelper();
    bool goCommand(string line);
    int estimateTimeToThink(int time_left, int time_inc);
    void execute(int time_to_wait);
    void stopCommand();
    bool processLine(string input);
};