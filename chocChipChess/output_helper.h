#pragma once
#include <fstream>
using namespace std;


class OutputHelper
{
private:
    string debugFilename;
    bool writeToFile;
    ofstream fileOut;
public:
    OutputHelper();
    OutputHelper(string filename);
    void output(string text);
    void debug(string text);
};

