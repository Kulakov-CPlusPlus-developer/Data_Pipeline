#ifndef DATA_PIPELINE_AGGREGATION_H
#define DATA_PIPELINE_AGGREGATION_H
#include <map>
#include <string>
#include <ctime>
#include <vector>
#include "LogEntry.h"
#include <set>


using namespace std;

class Aggregation {

public:
    void aggregating (vector<LogEntry> &entries);
private:
    float averageResponseTime;
    int countRequests = 0;
    int countErrorCode = 0;
    map<int, int> statusCount;
    map<string, int> topURL;
};

#endif //DATA_PIPELINE_AGGREGATION_H
