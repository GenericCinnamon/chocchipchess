#include "output_helper.h"
#include <iostream>


OutputHelper::OutputHelper()
{
    writeToFile = false;
}


OutputHelper::OutputHelper(string filename)
{
    debugFilename = filename;
    writeToFile = true;
    fileOut.open(debugFilename, ofstream::app);
    fileOut << "**************** Begin ****************" << endl;
    fileOut.close();
}


void OutputHelper::output(string text) {
    cout << text.c_str() << endl;
    debug(text);
}

void OutputHelper::debug(string text)
{
    if (writeToFile) {
        fileOut.open(debugFilename, ofstream::app);
        fileOut << text.c_str() << endl;
        fileOut.close();
    }
}

