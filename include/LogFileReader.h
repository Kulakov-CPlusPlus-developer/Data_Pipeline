#ifndef LOGFILEREADER_H
#define LOGFILEREADER_H

#include <vector>
#include <string>

using namespace std;

class LogFileReader {
public:
  LogFileReader(const string& fileName);

  vector<string> readLogFile();
private:
  string fileName_;
};


#endif //LOGFILEREADER_H
