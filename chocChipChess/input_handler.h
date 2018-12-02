#pragma once
#include <string>

using namespace std;

class InputHandler
{
public:
    virtual bool processLine(string input) = 0;
};

