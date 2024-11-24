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
    Tape* tape1; // tutaj zapisujemy serie (dłuższa taśma)
    Tape* tape2; // tutaj zapisujemy serie (krótsza taśma)
    Tape* tape3; // tutaj zapisujemy wynik scalania
    DataManager* dataManager;
    bool sorted;
    int phasesCount;
};



#endif //POLYPHASE_SORTING_H
