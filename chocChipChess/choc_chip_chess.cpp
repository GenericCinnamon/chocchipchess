// chocChipChess.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include "position.h"
#include <fstream>
#include "input_handler.h"
#include "uci_helper.h"
#include "sample_play.h"

int main()
{
    // Class to feed stdin to
    InputHandler* inputHandler = NULL;
 
    // Determine whether to start in uci or sample mode
    string input;
    getline(cin, input);

    if (input == "uci")
        inputHandler = new UCIHelper();
    else
        inputHandler = new SamplePlay();

    // Feed the input handler till it returns false
    while (inputHandler->processLine(input))
        getline(cin, input);

    return 0;
}

