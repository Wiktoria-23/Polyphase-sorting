#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H
#include <string>
#include <fstream>
#include <vector>

using namespace std;


class DataManager;
class DiskPage;
class Record;

class DataManager {
public:
    DataManager(string dataPath);
    virtual ~DataManager();
    virtual void startInput();
    virtual void stopInput();
    void writeRecordToFile(Record* record);
    virtual void printFile();
    virtual void startReadingData();
    void stopReadingData();
    bool isFileRead();
    Record* readRecordFromFile();
    void readNextDiskPageFromFile();
    virtual vector<Record*>* getNextRun();
    void writeDiskPageToFile();
    int getDiskAccessCounter();
    DiskPage* getCurrentDiskPage();
    void printRecords();
    const char* getFilename();
    virtual void createNewFile();
    void increaseReadingStart();
protected:
    DiskPage* currentDiskPage;
    string dataPath;
    fstream fileStream;
    bool continueReadingData;
    int diskAccessCounter;
    int readingStart;
};



#endif //DATA_MANAGER_H
