#ifndef LOGLINEPROCESSOR_H
#define LOGLINEPROCESSOR_H

#include <string>
#include "LogFileReader.h"
using namespace std;

class LogLineProcessor {
  public:
    void processLogLine(LogFileReader* reader);
};

#endif //LOGLINEPROCESSOR_H
