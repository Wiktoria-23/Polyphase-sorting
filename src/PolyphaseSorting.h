#ifndef POLYPHASE_SORTING_H
#define POLYPHASE_SORTING_H
#include <vector>
#include <limits>
using namespace std;

class DataManager;
class Tape;
class Record;

class PolyphaseSorting {
public:
    PolyphaseSorting(DataManager* dataManager);
    ~PolyphaseSorting();
    void readDataFromFile();
    void continueSorting();
    bool isSorted();
    void printTapes();
    void increasePhasesCount();
    void getReadyToSort();
    vector<Record*>* mergeRuns(vector<Record*>* run1, vector<Record*>* run2);
    int getPhasesCount();
    int countDiskAccesses();
    int calculateRunsAmount();
    void printResultAndDeleteFiles();
    void setNotSorted();
    void reset();
private:
    Tape* tape1; // longer tape
    Tape* tape2; // shorter tape
    Tape* tape3; // tape used to save results of merge
    DataManager* dataManager;
    bool sorted;
    int phasesCount;
};



#endif //POLYPHASE_SORTING_H
