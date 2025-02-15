#include "PolyphaseSorting.h"
#include "Tape.h"
#include "DataManager.h"
#include "DiskPage.h"
#include "Record.h"

PolyphaseSorting::PolyphaseSorting(DataManager* newDataManager) {
    tape1 = new Tape("../src/tape1.dat");
    tape2 = new Tape("../src/tape2.dat");
    tape3 = new Tape("../src/tape3.dat");
    dataManager = newDataManager;
    sorted = false;
    phasesCount = 0;
}

PolyphaseSorting::~PolyphaseSorting() {
    delete tape1;
    delete tape2;
    delete tape3;
}

void PolyphaseSorting::readDataFromFile() {
    // data distribution between tapes
    dataManager->startReadingData();
    dataManager->readNextDiskPageFromFile();
    vector<Record*>* run;
    // we set last elements on tape value as max double to indicate that tapes need to raise series limit
    // (start of new series)
    double lastElementValue1 = numeric_limits<double>::max();
    double lastElementValue2 = numeric_limits<double>::max();
    tape1->startInput();
    tape2->startInput();
    if (dataManager->getCurrentDiskPage()->getRecords()->empty()) {
        sorted = true;
        dataManager->stopReadingData();
        tape1->stopInput();
        tape2->stopInput();
        return;
    }
    do {
        run = dataManager->getNextRun();
        if (run == nullptr) {
            break;
        }
        double firstElementValue = run->at(0)->calculateField();
        if (tape1->getCapacity() > tape1->getRunsCount()) {
            tape1->writeRunToDiskPage(run, lastElementValue1 < firstElementValue);
            lastElementValue1 = run->back()->calculateField();
        }
        else if (tape2->getCapacity() > tape2->getRunsCount()) {
            tape2->writeRunToDiskPage(run, lastElementValue2 < firstElementValue);
            lastElementValue2 = run->back()->calculateField();
        }
        else if (tape1->getCapacity() <= tape2->getCapacity()) {
            tape1->increaseCapacity(tape2->getCapacity());
            tape1->writeRunToDiskPage(run, lastElementValue1 < firstElementValue);
            lastElementValue1 = run->back()->calculateField();
        }
        else {
            tape2->increaseCapacity(tape1->getCapacity());
            tape2->writeRunToDiskPage(run, lastElementValue2 < firstElementValue);
            lastElementValue2 = run->back()->calculateField();
        }
    } while (!dataManager->isFileRead()
        || dataManager->getCurrentDiskPage()->getIndex() < dataManager->getCurrentDiskPage()->getRecords()->size());
    cout << "Odczytano " << this->calculateRunsAmount() << " serii z pliku wejsciowego" << endl;
    getReadyToSort();
    cout << "W pliku do posortowania znajduje sie " << this->calculateRunsAmount() << " serii" << endl;
    dataManager->stopReadingData();
    tape1->stopInput();
    tape2->stopInput();
    if (tape1->getRunsCount() == 1 && tape2->getRunsCount() == 0) {
        sorted = true;
    }
}

void PolyphaseSorting::continueSorting() {
    // empty tape will be always tape3
    // the tape that has more series is tape2
    // shorter tape is tape1
    // after merge pointers to tapes are swapped so this condition is always true
    // all the series from tape1 are merged with the same amount of series from tape2
    // result of merge is saved on tape1
    tape1->startReadingData();
    tape1->readNextDiskPageFromFile();
    tape2->startReadingData();
    tape2->readNextDiskPageFromFile();
    tape3->startInput();
    int runsToMerge = tape1->getRunsCount();
    vector<Record*>* run1;
    vector<Record*>* run2;
    vector<Record*>* result;
    for (int i = 0; i < runsToMerge; i++) {
        run1 = this->tape1->getNextRun();
        run2 = this->tape2->getNextRun();
        if (run1 != nullptr && run2 != nullptr) {
            result = this->mergeRuns(run1, run2);
            tape3->writeRunToDiskPage(result, false);
        }
    }
    tape1->setCapacity(1);
    tape2->setCapacity(tape2->getRunsCount());
    tape3->setCapacity(tape3->getRunsCount());
    tape1->stopReadingData();
    tape2->stopReadingData();
    tape3->stopInput();
    // after sort:
    // empty tape - tape1
    // longer tape - tape3
    // shorter tape - tape2
    Tape* tmp = tape3;
    tape3 = tape1;
    tape1 = tape2;
    tape2 = tmp;
    tape3->createNewFile();
    if (calculateRunsAmount() == 1) {
        sorted = true;
    }
}

bool PolyphaseSorting::isSorted() {
    return sorted;
}

void PolyphaseSorting::printTapes() {
    cout << endl << "Tasma 1" << endl;
    tape1->printFile();
    cout << endl << "Tasma 2" << endl;
    tape2->printFile();
    cout << endl << "Tasma 3" << endl;
    tape3->printFile();
}

void PolyphaseSorting::increasePhasesCount() {
    phasesCount++;
}

void PolyphaseSorting::getReadyToSort() {
    // checking if tapes have proper length
    // empty series are added to tapes by record with field that's value is less than zero
    if (tape1->getCapacity() != tape1->getRunsCount() && tape1->getRunsCount() != 0) {
        while (tape1->getCapacity() != tape1->getRunsCount()) {
            vector<Record*>* emptyRun = new vector<Record *>{new Record(-1,-1,1)};
            tape1->writeRunToDiskPage(emptyRun, false);
        }
    }
    else if (tape2->getCapacity() != tape2->getRunsCount() && tape2->getRunsCount() != 0) {
        while (tape2->getCapacity() != tape2->getRunsCount()) {
            vector<Record*>* emptyRun = new vector<Record *>{new Record(-1,-1,1)};
            tape2->writeRunToDiskPage(emptyRun, false);
        }
    }
    if (tape1->getCapacity() > tape2->getCapacity()) {
        Tape* tmp = tape1;
        tape1 = tape2;
        tape2 = tmp;
    }
}

vector<Record *> *PolyphaseSorting::mergeRuns(vector<Record *> *run1, vector<Record *> *run2) {
    vector<Record*>* result = new vector<Record*>();
    if (run1 == nullptr && run2 == nullptr) {
        return nullptr;
    }
    if (run1 == nullptr) {
        return run2;
    }
    if (run2 == nullptr) {
        return run1;
    }
    while (!run1->empty() || !run2->empty()) {
        if (run2->empty() || (!run1->empty() && run1->front()->calculateField() <= run2->front()->calculateField())) {
            if (run1->front()->calculateField() > 0) {
                result->push_back(run1->front());
            }
            run1->erase(run1->begin());
        }
        else if (run1->empty()
            || (!run2->empty() && run2->front()->calculateField() <= run1->front()->calculateField())) {
            if (run2->front()->calculateField() > 0) {
                result->push_back(run2->front());
            }
            run2->erase(run2->begin());
        }
    }
    return result;
}

int PolyphaseSorting::getPhasesCount() {
    return phasesCount;
}

int PolyphaseSorting::countDiskAccesses() {
    return tape1->getDiskAccessCounter() + tape2->getDiskAccessCounter() + tape3->getDiskAccessCounter();
}

int PolyphaseSorting::calculateRunsAmount() {
    return (tape1->getRunsCount() + tape2->getRunsCount() + tape3->getRunsCount());
}

void PolyphaseSorting::printResultAndDeleteFiles() {
    if (tape2->getRunsCount() != 0) {
        tape2->printFile();
    }
    else {
        tape1->printFile();
    }
    string path = dataManager->getFilename();
    remove(dataManager->getFilename());
    if (tape2->getRunsCount() > 0) {
        rename(tape2->getFilename(), path.c_str());
        remove(tape1->getFilename());
    }
    else {
        rename(tape1->getFilename(), path.c_str());
        remove(tape2->getFilename());
    }
    remove(tape3->getFilename());
    tape1->createNewFile();
    tape2->createNewFile();
    tape3->createNewFile();
}

void PolyphaseSorting::setNotSorted() {
    sorted = false;
}

void PolyphaseSorting::reset() {
    sorted = false;
    phasesCount = 0;
    string path = dataManager->getFilename();
    delete dataManager;
    dataManager = new DataManager(path);
    tape1->reset();
    tape2->reset();
    tape3->reset();
}