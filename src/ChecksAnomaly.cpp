#include "Anomaly.h"
#include "nlohmann/json.hpp"
#include <fstream>
#include <ctime>
#include <iomanip>
#include <iostream>

using namespace std;

Anomaly_container::Anomaly_container() {

}

Anomaly_container::~Anomaly_container() {

}

void Anomaly_container::check(float& response_time, time_t& timestamp, string& ipAdress, string& url, string& time_zone) {

    if(response_time/1000000 > 2){
        nlohmann::json anomalyNumber;
        nlohmann::json infoLog;
        count += 1;
        tm* t = localtime(&timestamp);
        stringstream buffer;
        buffer << put_time(t, "%d/%b/%Y:%H:%M:%S");
        string time;
        getline(buffer, time);
        infoLog["timestamp"] = time + time_zone;
        infoLog["IP"] = ipAdress;
        infoLog["URL"] = url;
        infoLog["Response time"] = response_time/1000000;
        anomalyNumber["#" + to_string(count)] = infoLog;

        anomaly["LONG_PROCESSING_TIME"].emplace_back(anomalyNumber);



        if(count >= 10) {
            time_t now = std::time(nullptr);
            tm* local = localtime(&now);
            stringstream buf;
            buf << put_time(local, "%d-%b-%Y");
            getline(buf, countFile);
            ofstream anomalyFile;
            anomalyFile.open("../data/anomalyFile_" + countFile + ".json");
            anomalyFile << anomaly.dump(4);
            anomalyFile.close();
            anomaly.clear();
            count = 0;
        }
    }
}