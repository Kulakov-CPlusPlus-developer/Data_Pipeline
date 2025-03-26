#include "Aggregation.h"
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>


using namespace std;

void Aggregation::aggregating (vector<LogEntry> &entries) {
    int count = 0;
    int count400 = 0;
    int count500 = 0;

    time_t oneHourAgo = time(nullptr) - 3600;
    time_t now = time(nullptr);

    mutex _mtx;
    std::vector<std::thread> threads;
    size_t num_threads = std::thread::hardware_concurrency();
    size_t log_per_theard = entries.size() / num_threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; i++) {
        size_t start = i * log_per_theard;
        size_t end = (i == num_threads - 1) ? entries.size() : (i + 1) * log_per_theard;
        threads.emplace_back([this, start, end, &_mtx, &entries, &count, &oneHourAgo, &now, &count400, &count500]() {
            int local400 = 0;
            int local500 = 0;
            float localAverageResponseTime = 0.0;
            int localCount = 0;
            int localCountError = 0;
            map<string, int> localTopUrl;
            int localCountRec = 0;

            for (size_t token_id = start; token_id < end; ++token_id) {

                if (entries[token_id].timestamp >= oneHourAgo) {

                    localAverageResponseTime += entries[token_id].response_time;
                    localCount++;


                    if (entries[token_id].statusCode / 400 >= 1 || entries[token_id].statusCode / 500 >= 1) {

                        localCountError++;

                        if (entries[token_id].statusCode / 400 >= 1) {

                            local400++;

                        }
                        if (entries[token_id].statusCode / 500 >= 1) {

                            local500++;

                        }

                    }
                    if (entries[token_id].response_time / 1000000 < 2) {

                        if (localTopUrl.count(entries[token_id].url) == 1) {
                            localTopUrl[entries[token_id].url]++;
                        } else {
                            localTopUrl[entries[token_id].url] = 1;
                        }

                    }
                    if (entries[token_id].timestamp == now) {

                        localCountRec++;

                    }

                }

            }
            std::lock_guard<std::mutex> lock(_mtx);
            averageResponseTime += localAverageResponseTime;
            count += localCount;
            countErrorCode += localCountError;
            count400 += local400;
            count500 += local500;
            countRequests += localCountRec;

            for(auto& pair : localTopUrl) {
                topURL[pair.first] += pair.second;
            }
            statusCount[400] = count400;
            statusCount[500] = count500;
        });


    }

    for (auto& thread : threads) {
        thread.join();
    }

    statusCount[400] = count400;
    statusCount[500] = count500;
    averageResponseTime = (averageResponseTime/(float)count)/1000000;
}
