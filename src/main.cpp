#include <iostream>
#include "LogFileReader.h"
#include "LogLineProcessor.h"
#include <string>
#include "tinyfiledialogs.h"
using namespace std;

std::string OpenFileDialog() {
    const char * filters[] = {"*.log", "*.txt", "*"};
    const char * filePath = tinyfd_openFileDialog(
        "Open Log File",
        "../data",
        3,
        filters,
        "Log files",
        0
    );
    if (filePath) {
        return string(filePath);
    } else {
        return "";
    }
}




int main() {
    cout << "Data Pipeline started!" << endl;
    string filePath = OpenFileDialog();
    if (!filePath.empty()) {
        cout << "Selected file: " << filePath << endl;
        LogFileReader reader(filePath);
        LogLineProcessor processor;

        vector<std::string> lines = reader.readLogFile();
        processor.processLogLine(&reader);
    } else {
        cout << "No file selected." << endl;
    }
    cout << "Data Pipeline finished!" << endl;
    return 0;
}
