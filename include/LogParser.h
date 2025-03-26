#ifndef LOGPARSER_H
#define LOGPARSER_H

#include <LogFileReader.h>
#include <vector>
#include "LogEntry.h"

class LogParser {
  public:
  vector<LogEntry> parseLine(LogFileReader* reader);
};
#endif //LOGPARSER_H
