#include "LogParser.h"
#include "LogFileReader.h"
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <ctime>
#include <thread>
#include <mutex>
#include "Anomaly.h"
#include "Aggregation.h"
using namespace std;

vector<LogEntry> LogParser::parseLine(LogFileReader* reader) {

    Anomaly_container container;
    vector<LogEntry> entries;
    vector<string> tokens = reader->readLogFile();

    mutex mtx;
    std::vector<std::thread> threads;
    size_t num_threads = std::thread::hardware_concurrency();
    size_t log_per_theard = tokens.size() / num_threads;
    threads.reserve(num_threads);
    for (int i = 0; i < num_threads; i++) {
        size_t start = i * log_per_theard;
        size_t end = (i == num_threads - 1) ? tokens.size() : (i + 1) * log_per_theard;
        threads.emplace_back([this, start, end, &mtx, &entries, &tokens, &container]() {
            for (size_t token_id = start; token_id < end; ++token_id ) {


                string temp;
                LogEntry entry;

                mtx.lock();
                stringstream ss(tokens[token_id]);
                mtx.unlock();

                ss >> entry.ipAdress;

                ss >> temp;
                ss >> temp;


                string timestampStr;

                getline(ss, timestampStr, ']');
                timestampStr = timestampStr.substr(1);
                timestampStr = timestampStr.substr(1);
                tm t{};
                istringstream timeStream(timestampStr);
                timeStream >> get_time(&t, "%d/%b/%Y:%H:%M:%S");
                timeStream >> ws;

                timeStream >> entry.time_zone;

                if (timeStream.fail()) {
                    cerr << "Failed to parse time string: " << timestampStr << endl;

                    entry.timestamp = 0;

                } else {

                    entry.timestamp = mktime(&t);

                }

                string requestStr;
                ss >> ws;
                getline(ss, requestStr, '"');
                getline(ss, requestStr, '"');

                istringstream requestStream(requestStr);

                requestStream >> entry.method >> entry.url >> temp;


                ss >> entry.statusCode >> entry.responseSize;

                ss >> ws;
                string refererStr;
                getline(ss, refererStr, '"');
                getline(ss, refererStr, '"');
                istringstream refererStream(refererStr);

                refererStream >> entry.referer;

                ss >> ws;
                string userAgentStr;
                getline(ss, userAgentStr, '"');
                getline(ss, userAgentStr, '"');
                istringstream userAgentStream(userAgentStr);

                getline(userAgentStream, entry.userAgent);

                ss >> ws;
                string response_time;
                ss >> response_time;

                entry.response_time = stof(response_time);

                ss >> ws;
                string cookie;

                getline(ss, cookie, '"');
                getline(ss, cookie, '"');
                istringstream cookieStream(cookie);
                getline(cookieStream, entry.cookie);

                mtx.lock();
                container.check(entry.response_time, entry.timestamp, entry.ipAdress, entry.url, entry.time_zone);
                mtx.unlock();

                mtx.lock();
                entries.push_back(entry);
                mtx.unlock();
            }
        });
    }
    for(auto& thread : threads){
        thread.join();
    }
    Aggregation aggregation;
    aggregation.aggregating(entries);
    return entries;
}
