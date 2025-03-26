#ifndef LOGENTRY_H
#define LOGENTRY_H

#include <string>
#include <ctime>
using namespace std;

struct LogEntry {
  string ipAdress;
  time_t timestamp;
  string time_zone;
  string method;
  string url;
  int statusCode;
  long long responseSize;
  string referer;
  string userAgent;
  float response_time;
  string cookie;
};
#endif //LOGENTRY_H
