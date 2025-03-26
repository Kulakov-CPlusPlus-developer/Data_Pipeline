#ifndef DATA_PIPELINE_ANOMALY_H
#define DATA_PIPELINE_ANOMALY_H
#include "nlohmann/json.hpp"

using namespace std;

class Anomaly_container {
public:
    Anomaly_container();
    ~Anomaly_container();
    void check(float& response_time, time_t& timestamp, string& ipAdress, string& url, string& time_zone);
private:
    nlohmann::json anomaly;
    size_t count = 0;
    string countFile;

};


#endif //DATA_PIPELINE_ANOMALY_H
