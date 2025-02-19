#ifndef DISKPAGE_H
#define DISKPAGE_H
#include <vector>
using namespace std;

class Record;

class DiskPage {
private:
    int capacity;
    vector<Record*>* records;
    bool full;
    int index;
public:
    DiskPage();
    ~DiskPage();
    vector<Record*>* getRecords();
    void clear();
    void writeRecordToDiskPage(Record* record);
    bool isFull();
    void increaseIndex();
    int getIndex();
    void deleteAllRecords();
    Record* getRecordFromDiskPage(int index);
    Record* getNextRecordFromDiskPage();
};



#endif //DISKPAGE_H
