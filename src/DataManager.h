#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;


class DataManager;
class DiskPage;
class Record;

class DataManager {
public:
    DataManager(string dataPath);
    ~DataManager();
    virtual void startInput();
    void stopInput();
    void writeRecordToFile(Record* record);
    virtual void printFile();
    virtual void startReadingData();
    void stopReadingData();
    bool isFileRead();
    Record* readRecordFromFile();
    void readNextDiskPageFromFile();
    virtual vector<Record*>* getNextRun();
    void increaseDiskAccessCounter();
    void writeDiskPageToFile();
    int getDiskAccessCounter();
    DiskPage* getCurrentDiskPage();
    void printRecords();
    const char* getFilename();
    virtual void createNewFile();
protected:
    DiskPage* currentDiskPage;
    string dataPath;
    fstream fileStream;
    bool continueReadingData;
    int diskAccessCounter;
};



#endif //DATA_MANAGER_H
