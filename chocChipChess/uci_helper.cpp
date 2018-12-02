#include "uci_helper.h"
#include <iostream>
#include <sstream>
#include <thread>
#include "board_definitions.h"


/*
UCIHelper constructor - Replies with engine name and uciok on creation
*/
UCIHelper::UCIHelper()
{
    // Debug file
    output = OutputHelper("ccchess-engine.log");

    // Send the standard UCI protocol startup messages
    char buffer[100];
    sprintf_s(buffer, "id name ChocChipChess %s %s", __DATE__, __TIME__);
    sendToGUI(buffer);
    sendToGUI("uciok");
}

/*
sendToGUI - Output a line back to the gui
*/
void UCIHelper::sendToGUI(string line)
{
    output.output(line);
}

/*
initNewGame - Reset the board and ponder thread
*/
void UCIHelper::initNewGame() {
    if (pos != NULL) {
        delete pos;
        pos = NULL;
    }
    if (ponderBoard != NULL) {
        delete ponderBoard;
        ponderBoard = NULL;
    }
}

/*
loadPosition - Load a position from a FEN string
*/
void UCIHelper::loadPosition(string fen)
{
    initNewGame();
    pos = new Position(fen, &sharedTT);
    pos->directOutput(&output);
    ponderBoard = new Position(fen, &sharedTT);
    ponderBoard->directOutput(&output);
}

/*
positionCommand - Parse the UCI position command - load a position
*/
void UCIHelper::positionCommand(string line)
{
    istringstream iss(line);

    string word;
    int word_count = 0;
    while (getline(iss, word, ' ')) {
        word_count++;
        switch (word_count) {
        case 1:
            initNewGame();
            break;
        case 2:
            if (word == "startpos")
                loadPosition(STARTPOS_FEN);
            else if (word == "fen") {
                getline(iss, word, ' ');
                for (int i = 0; i < 5; i++) {
                    string word1;
                    getline(iss, word1, ' ');
                    word += " " + word1;
                }

                loadPosition(word.c_str());
            }
            break;
        case 3:
            if (word == "moves")
                while (getline(iss, word, ' ')) {
                    if (!pos->inputMove(word)) {
                        sendToGUI("bad move given: " + word);
                        break;
                    }
                    if (!ponderBoard->inputMove(word))
                        throw logic_error("Ponder board couldn't perform a move the main board could.");
                }
            return;
        }
    }
}

/*
stopPonder - Kill the ponder thread
*/
void UCIHelper::stopPonder()
{
    // If we aren't pondering then return
    if (ponderThread == NULL)
        return;

    // Tell the board to stop thinking
    ponderBoard->stopThinking = true;
    // Wait for the thread to stop
    ponderThread->join();
    // Allow thinking again
    ponderBoard->stopThinking = false;

    // Delete the pondering thread
    delete ponderThread;
    ponderThread = NULL;
}


/*
goCommand - Respond to UCI:go
*/
bool UCIHelper::goCommand(string line) {
    istringstream iss(line);

    int wtime = 60000;
    int btime = 60000;
    int winc = 0;
    int binc = 0;

    string word;
    int word_count = 0;
    while (getline(iss, word, ' ')) {
        word_count++;
        if (word == "wtime") {
            if (!getline(iss, word, ' '))
                return false;
            wtime = stoi(word);
        }
        else if (word == "btime") {
            if (!getline(iss, word, ' '))
                return false;
            btime = stoi(word);
        }
        else if (word == "winc") {
            if (!getline(iss, word, ' '))
                return false;
            winc = stoi(word);
        }
        else if (word == "binc") {
            if (!getline(iss, word, ' '))
                return false;
            binc = stoi(word);
        }
        else if (word == "infinite") {
            wtime = -1;
            btime = -1;
        }
    }

    int time = pos->sideToMove == WHITE ? wtime : btime;
    int inc = pos->sideToMove == WHITE ? winc : binc;

    stopPonder();
    ponderThread = new thread(&Position::ponder, ponderBoard, 0);
    if (time > -1)
        new thread(&UCIHelper::execute, this, estimateTimeToThink(time, inc));
    return true;
}

/*
estimateTimeToThink - Estimate how long to ponder this move
*/
int UCIHelper::estimateTimeToThink(int time_left, int time_inc) {
    return time_left / AVERAGE_GAME_LENGTH + time_inc / 2;
}

/*
execute - Wait for the given time then perform a stop
*/
void UCIHelper::execute(int time_to_wait) {
    std::this_thread::sleep_for(std::chrono::milliseconds(time_to_wait));
    stopCommand();
}

/*
stopCommand - Stop pondering and return the current top move
*/
void UCIHelper::stopCommand() {
    stopPonder();
    Move* best_move = pos->topMove();
    sendToGUI("bestmove " + best_move->getName());
    sendToGUI("info score cp " + to_string(best_move->eval));
    if (abs(best_move->eval) > CHECKMATE_THRESHOLD)
        sendToGUI("info score mate " + to_string(CHECKMATE_VALUE - abs(best_move->eval)));
}

/*
handleLine - Parse a UCI command
*/
bool UCIHelper::processLine(string input)
{
    istringstream iss(input);

    // Get the first word in the line
    string first_word;
    getline(iss, first_word, ' ');
    
    // Log the line
    output.debug(input);

    // UCI Protocol commands
    // isready    - check whether the engine is in a ready state, always reply with 'readyok'
    if (first_word == "isready")
        sendToGUI("readyok");
    // ucinewgame - start a new game
    else if (first_word == "ucinewgame")
        initNewGame();
    // position - set the board to the specified position
    else if (first_word == "position")
        positionCommand(input);
    // go        - start thinking
    else if (first_word == "go") {
        if (pos != NULL) {
            goCommand(input);
        }
        else {
            // If a position hasn't been specified then there's nothing to think about
            sendToGUI("bad");
        }
    }
    // stop        - stop thinking and output the current best move
    else if (first_word == "stop") {
        stopCommand();
    }
    // Custom commands

    // quit        - quit the program
    else if (first_word == "quit") {
        return false;
    }
    // d | ? | print | p - print out the board
    else if (first_word == "d" || first_word == "?" || first_word == "print" || first_word == "p") {
        if (pos != NULL)
            output.output(pos->printBoard());
        else if (first_word == "stop")
            stopCommand();
    }
    return true;
}

