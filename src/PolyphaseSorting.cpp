#include <iostream>
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
    // we set last elements on tape value as max double to indicate that tapes need to raise series limit
    // (start of new series)
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
        if ((tape1->checkIfRunEnds(dataManager) && tape1->getCapacity() > tape1->getRunsCount() + 1)
            || (!tape1->checkIfRunEnds(dataManager) && tape1->getCapacity() > tape1->getRunsCount())) {
            copyRunToAnotherTape(dataManager, tape1);
        }
        else if ((tape2->checkIfRunEnds(dataManager) && tape2->getCapacity() > tape2->getRunsCount() + 1)
            || (!tape2->checkIfRunEnds(dataManager) && tape2->getCapacity() > tape2->getRunsCount())) {
            copyRunToAnotherTape(dataManager, tape2);
        }
        else if (tape1->getCapacity() <= tape2->getCapacity()) {
            tape1->increaseCapacity(tape2->getCapacity());
            copyRunToAnotherTape(dataManager, tape1);
        }
        else {
            tape2->increaseCapacity(tape1->getCapacity());
            copyRunToAnotherTape(dataManager, tape2);
        }
    } while (!dataManager->getCurrentDiskPage()->getRecords()->empty()
        || dataManager->getCurrentDiskPage()->getIndex() < dataManager->getCurrentDiskPage()->getRecords()->size());
    dataManager->stopReadingData();
    tape1->stopInput();
    tape2->stopInput();
    cout << "Odczytano " << this->calculateRunsAmount() << " serii z pliku wejsciowego" << endl;
    getReadyToSort();
    cout << "W pliku do posortowania znajduje sie " << this->calculateRunsAmount() << " serii" << endl;
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
    mergeRuns();
    tape1->setCapacity(tape1->getRunsCount());
    tape2->setCapacity(1);
    tape3->setCapacity(tape3->getRunsCount());
    tape1->stopReadingData();
    tape2->stopReadingData();
    tape3->stopInput();
    // after merge:
    // shorter tape - tape1
    // empty tape - tape2
    // longer tape - tape3
    Tape* tmp = tape3;
    tape3 = tape2;
    tape2 = tape1;
    tape1 = tmp;
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
    cout << "Na tasmie 1 znajduje sie " << tape1->getRunsCount() << " serii w tym " << tape1->getFakeRunsCount();
    cout << " serii pustych";
    cout << endl << "Tasma 2" << endl;
    tape2->printFile();
    cout << "Na tasmie 2 znajduje sie " << tape2->getRunsCount() << " serii w tym " << tape2->getFakeRunsCount();
    cout << " serii pustych";
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
            tape1->increaseRunsCount();
            tape1->increaseFakeRunsCount();
        }
    }
    else if (tape2->getCapacity() != tape2->getRunsCount() && tape2->getRunsCount() != 0) {
        while (tape2->getCapacity() != tape2->getRunsCount()) {
            tape2->increaseRunsCount();
            tape2->increaseFakeRunsCount();
        }
    }
    if (tape1->getCapacity() < tape2->getCapacity()) {  // zmieniony znak równości - nie wiem dlaczego to działało
        Tape* tmp = tape1;
        tape1 = tape2;
        tape2 = tmp;
    }
}

void PolyphaseSorting::mergeRuns() {
    int runsToMerge = tape2->getRunsCount();
    while (runsToMerge > 0) {
        tape1->decreaseRunsCount();
        tape2->decreaseRunsCount();
        tape1->setLastElementValue(0);
        tape2->setLastElementValue(0);
        tape3->setLastElementValue(0);
        if (tape1->getFakeRunsCount() > 0) {
            // run from tape2 is copied as we merge it with empty run from tape1
            tape1->decreaseFakeRunsCount();
            tape3->startNewRun();
            copyRunToAnotherTape(tape2, tape3);
        }
        else {
            if (tape3->getLastElementValue() == 0) {
                tape3->startNewRun();
            }
            while (true) {
                Record* readRecord1 = tape1->getCurrentDiskPage()->getNextRecordFromDiskPage();
                Record* readRecord2 = tape2->getCurrentDiskPage()->getNextRecordFromDiskPage();
                if (readRecord1 != nullptr && readRecord1->calculateField() > tape1->getLastElementValue()
                    &&  readRecord2 != nullptr && readRecord2->calculateField() > tape2->getLastElementValue())
                    {
                    Record* toWrite;
                    if (readRecord1->calculateField() < readRecord2->calculateField()) {
                        toWrite = readRecord1;
                        tape1->getCurrentDiskPage()->increaseIndex();
                        tape1->setLastElementValue(readRecord1->calculateField());
                        tape1->increaseReadingStart();
                    }
                    else {
                        toWrite = readRecord2;
                        tape2->getCurrentDiskPage()->increaseIndex();
                        tape2->setLastElementValue(readRecord2->calculateField());
                        tape2->increaseReadingStart();
                    }
                    tape3->getCurrentDiskPage()->writeRecordToDiskPage(toWrite);
                }
                else {
                    if (readRecord1 != nullptr && readRecord1->calculateField() > tape1->getLastElementValue()) {
                        copyRunToAnotherTape(tape1, tape3);
                    }
                    else if (readRecord2 != nullptr && readRecord2->calculateField() > tape2->getLastElementValue()) {
                        copyRunToAnotherTape(tape2, tape3);
                    }
                    break;
                }
            }
        }
        runsToMerge--;
    }
    // ending last run on a tape
    tape3->setLastElementValue(0);
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

void PolyphaseSorting::copyRunToAnotherTape(DataManager* srcTape, Tape* destTape) {
    DiskPage* diskPageTape = srcTape->getCurrentDiskPage();
    if (destTape->checkIfRunEnds(srcTape)) {
        destTape->startNewRun();
    }
    while (true) {
        Record* readRecord = diskPageTape->getNextRecordFromDiskPage();
        if (readRecord != nullptr && readRecord->calculateField() > destTape->getLastElementValue()) {
            diskPageTape->increaseIndex();
            srcTape->increaseReadingStart();
            destTape->getCurrentDiskPage()->writeRecordToDiskPage(readRecord);
            destTape->setLastElementValue(readRecord->calculateField());
            if (diskPageTape->getIndex() >= diskPageTape->getRecords()->size()) {
                srcTape->readNextDiskPageFromFile();
                if (diskPageTape->getRecords()->empty()) {
                    break;
                }
            }
        }
        else {
            break;
        }
    }
}
