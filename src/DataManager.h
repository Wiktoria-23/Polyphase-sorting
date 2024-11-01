#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <string>
#include <fstream>
#include <iostream>
#include "Record.h"
using namespace std;


class DataManager {
public:
    DataManager();
    ~DataManager();
    void startInput();
    void stopInput();
    void writeRecordToFile(Record* record);
    void printFile();
private:
    string dataPath = "../src/data.dat";
    fstream fileStream;
};



#endif //DATA_MANAGER_H
