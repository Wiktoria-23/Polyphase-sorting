#ifndef TAPE_H
#define TAPE_H
#include <string>
#include <cstdio>
#include <vector>
#include "DataManager.h"
using namespace std;

class DataManager;
class Record;

class Tape: public DataManager {
public:
    Tape(string dataPath);
    ~Tape();
    void increaseCapacity(int toAdd);
    int getCapacity();
    void writeRunToDiskPage(vector<Record*>* run, bool continueRun);
    int getRunsCount();
    vector<Record*>* getNextRun() override;
    void startReadingData() override;
    void startInput() override;
    void printFile() override;
    void setCapacity(int newCapacity);
    void createNewFile() override;
    void reset();
private:
    int currentCapacity;
    int runsCount;
    int readingStart;
};



#endif //TAPE_H
