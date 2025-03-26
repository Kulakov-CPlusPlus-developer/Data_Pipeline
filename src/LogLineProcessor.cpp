#include "LogLineProcessor.h"

#include <cmath>
#include <iostream>
#include <LogParser.h>
#include <math.h>


void LogLineProcessor::processLogLine(LogFileReader* reader) {
  LogParser parser;
  vector<LogEntry> entries = parser.parseLine(reader);

  if (entries.size() != 0) {
      for(auto entry : entries) {
          cout << "-----------------------------------------------------" << endl;
          cout << "-----------------------------------------------------" << endl;
          cout << "IP address: " << entry.ipAdress << endl;
          cout << "Timestamp: " << entry.timestamp << endl;
          cout << "Time zone: " << entry.time_zone << endl;
          cout << "Method: " << entry.method << endl;
          cout << "URL: " << entry.url << endl;
          cout << "Status: " << entry.statusCode;
          cout << "Response size: " << entry.responseSize << endl;
          cout << "Referer: " << entry.referer << endl;
          cout << "User agent: " << entry.userAgent << endl;
          cout << "Response time: " << entry.response_time / 1000000 << " seconds" << endl;
          cout << "Cookie: " << entry.cookie << endl;
      }
  }
}