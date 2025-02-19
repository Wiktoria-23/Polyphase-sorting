#include "Tape.h"

#include <utility>
#include "DiskPage.h"
#include "Program.h"
#include "Record.h"

Tape::Tape(string dataPath): DataManager(std::move(dataPath)) {
    currentCapacity = 1;
    runsCount = 0;
    readingStart = 0;
    fakeRunsCount = 0;
    lastElementValue = 0;
}

Tape::~Tape() {
    remove(dataPath.c_str());
}

void Tape::increaseCapacity(int toAdd) {
    currentCapacity += toAdd;
}

int Tape::getCapacity() {
    return currentCapacity;
}

int Tape::getRunsCount() {
    return runsCount;
}

void Tape::startReadingData() {
    DataManager::startReadingData();
    fileStream.seekg(readingStart);
}

void Tape::startInput() {
    DataManager::startInput();
    fileStream.seekp(readingStart);
}

void Tape::printFile() {
    startReadingData();
    fileStream.seekg(readingStart);
    printRecords();
    stopReadingData();
}

void Tape::setCapacity(int newCapacity) {
    currentCapacity = newCapacity;
}

void Tape::createNewFile() {
    DataManager::createNewFile();
    readingStart = 0;
}

void Tape::reset() {
    diskAccessCounter = 0;
    runsCount = 0;
}

void Tape::increaseRunsCount() {
    runsCount++;
}

void Tape::decreaseRunsCount() {
    runsCount--;
}

void Tape::increaseFakeRunsCount() {
    fakeRunsCount++;
}

void Tape::decreaseFakeRunsCount() {
    fakeRunsCount--;
}

int Tape::getFakeRunsCount() {
    return fakeRunsCount;
}

void Tape::setLastElementValue(double value) {
    lastElementValue = value;
}

double Tape::getLastElementValue() {
    return lastElementValue;
}

void Tape::stopInput() {
    DataManager::stopInput();
    // if last element value is not equal zero, it means that there is run started on tape
    if (lastElementValue != 0) {
        increaseRunsCount();
    }
}

bool Tape::checkIfRunEnds(DataManager* srcTape) {
    DiskPage* diskPageTape = srcTape->getCurrentDiskPage();
    Record* readRecord = diskPageTape->getNextRecordFromDiskPage();
    if (readRecord != nullptr && readRecord->calculateField() < getLastElementValue()) {
        // starting new run
        return true;
    }
    return false;
}

void Tape::startNewRun() {
    increaseRunsCount();
    setLastElementValue(0);
}
