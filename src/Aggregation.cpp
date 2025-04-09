#include "Aggregation.h"
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <thread>
#include <libpq-fe.h>
#include <fstream>


using namespace std;

string readPasswordFromFile(string filename) {
    ifstream configFile(filename);
    string line;
    while (getline(configFile, line)) {
        size_t pos = line.find("database_password=");
        if (pos != string::npos) {
            return line.substr(pos + string("database_password=").length());
        }
    }
    return "NULL";
}

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

                    localCountRec++;

                }

            }
            std::lock_guard<std::mutex> lock(_mtx);
            averageResponseTime += localAverageResponseTime;
            count += localCount;
            countErrorCode += localCountError;
            count400 += local400;
            count500 += local500;
            countRequests += localCountRec;

            for (auto &pair: localTopUrl) {
                topURL[pair.first] += pair.second;
            }
            statusCount[400] = count400;
            statusCount[500] = count500;
        });
    }
    for(auto& thread : threads){
        thread.join();
    }

    averageResponseTime = (averageResponseTime/(float)count)/1000000;

    string password = readPasswordFromFile("../data/config.txt");
    const char* pass = password.c_str();

    const char* keywords [] = {
            "host",
            "port",
            "dbname",
            "user",
            "password",
            NULL
    };

    const char* values [] = {
            "localhost",
            "5432",
            "postgres",
            "postgres",
            pass,
            NULL
    };
    PGconn* conn = PQconnectdbParams(keywords, values, 0);

    if(PQstatus(conn) == CONNECTION_BAD) {
        cerr << "Connection to database failed: " << PQerrorMessage(conn) << endl;
        PQfinish(conn);
    }else {

        tm *t = localtime(&now);
        stringstream buf;
        buf << put_time(t, "%d/%b/%Y:%H:%M:%S");
        string timeString = buf.str();

        string topURLString;
        if(!topURL.empty()){
            topURLString = topURL.begin()->first;
            for(auto& url : topURL){
                if(url.second > topURL[topURLString]){
                    topURLString = url.first;
                }
            }
        }else {
            topURLString = "NULL";
        }

        const char* createTableSQL = "CREATE TABLE IF NOT EXISTS sensor_data (\
            time TIMESTAMP,\
            averageResponseTime FLOAT,\
            countRequests INTEGER,\
            countErrorCode INTEGER,\
            statusCount400 INTEGER,\
            statusCount500 INTEGER,\
            topURL TEXT\
            )";

        PGresult* createRes = PQexec(conn, createTableSQL);
        if (PQresultStatus(createRes) != PGRES_COMMAND_OK) {
            cerr << "Failed to create table: " << PQerrorMessage(conn) << endl;
            PQclear(createRes);
            PQfinish(conn);
            return;
        }
        PQclear(createRes);

        const char* stmtName = "insert_sensor_data";
        const char* sql = "INSERT INTO sensor_data (time, averageResponseTime, countRequests, countErrorCode, statusCount400, statusCount500, topURL) VALUES ($1, $2, $3, $4, $5, $6, $7)";

        PGresult* prepareRes = PQprepare(conn, stmtName, sql, 7, NULL);

        if (PQresultStatus(prepareRes) != PGRES_COMMAND_OK) {
            cerr << "SQL command failed: " << PQerrorMessage(conn) << endl;
            PQclear(prepareRes);
            PQfinish(conn);
        } else cout << "Prepared statement created successfully!" << endl;

        PQclear(prepareRes);

        const char* paramValues[7] = {
                timeString.c_str(),
                to_string(averageResponseTime).c_str(),
                to_string(countRequests).c_str(),
                to_string(countErrorCode).c_str(),
                to_string(statusCount[400]).c_str(),
                to_string(statusCount[500]).c_str(),
                topURLString.c_str()
        };

        int paramLengths[7] = {
                static_cast<int>(timeString.length()),
                static_cast<int>(to_string(averageResponseTime).length()),
                static_cast<int>(to_string(countRequests).length()),
                static_cast<int>(to_string(countErrorCode).length()),
                static_cast<int>(to_string(statusCount[400]).length()),
                static_cast<int>(to_string(statusCount[500]).length()),
                static_cast<int>(topURLString.length())
        };

        int paramFormats[7] = {0, 0, 0, 0, 0, 0, 0};

        PGresult* execParamsRes = PQexecPrepared(conn,
                                               stmtName,
                                               7,
                                               paramValues,
                                               paramLengths,
                                               paramFormats,
                                               0);

        if (PQresultStatus(execParamsRes) != PGRES_COMMAND_OK) {
            cerr << "Execute statement failed: " << PQerrorMessage(conn) << endl;
            PQclear(execParamsRes);
            PQfinish(conn);
        } else {
            cout << "Data inserted successfully!" << endl;
        }

        PQclear(execParamsRes);
        PQfinish(conn);
    }
}
