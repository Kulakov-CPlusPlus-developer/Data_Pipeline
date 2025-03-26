#include "LogFileReader.h"
#include <fstream>
#include <iostream>

using namespace std;

LogFileReader::LogFileReader(const string &fileName): fileName_(fileName) {}

vector<string> LogFileReader::readLogFile() {
    vector<string> lines;
    ifstream logFile(fileName_);
    string line;
    if (logFile.is_open()) {
        while (getline(logFile, line)) {
            lines.push_back(line);
        }
    }else {
        cout << "Unable to open file " << fileName_ << endl;
    }
    return lines;;
}
