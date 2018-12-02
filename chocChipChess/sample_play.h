#pragma once
#include "input_handler.h"
#include "position.h"

class SamplePlay : public InputHandler
{
public:
    SamplePlay();
    bool processLine(string input);
private:
    Position* pos;
};

