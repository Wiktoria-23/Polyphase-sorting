#ifndef TAPE_H
#define TAPE_H
#include <string>
#include <cstdio>
#include <vector>
#include "DataManager.h"
using namespace std;

class Record;

class Tape: public DataManager {
public:
    Tape(string dataPath);
    ~Tape();
    void increaseCapacity(int toAdd);
    void increaseRunsCount();
    void increaseFakeRunsCount();
    void decreaseRunsCount();
    void decreaseFakeRunsCount();
    int getCapacity();
    int getRunsCount();
    int getFakeRunsCount();
    void startReadingData() override;
    void startInput() override;
    void printFile() override;
    void setCapacity(int newCapacity);
    void createNewFile() override;
    void reset();
    void setLastElementValue(double value);
    double getLastElementValue();
    void stopInput() override;
    bool checkIfRunEnds(DataManager* srcTape);
    void startNewRun();
private:
    int currentCapacity;
    int runsCount;
    int fakeRunsCount;
    double lastElementValue;
};



#endif //TAPE_H
