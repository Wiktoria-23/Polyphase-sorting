#include "Tape.h"
#include "DataManager.h"
#include "DiskPage.h"
#include "Program.h"

Tape::Tape(string dataPath): DataManager(dataPath) {
    currentCapacity = 1;
    runsCount = 0;
    readingStart = 0;
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

void Tape::writeRunToDiskPage(vector<Record*>* run, bool continueRun) {
    for (int i = 0; i < run->size(); i++) {
        if (currentDiskPage->isFull()) {
            writeDiskPageToFile();
        }
        currentDiskPage->writeRecordToDiskPage(run->at(i));
    }
    if (!continueRun) {
        runsCount++;
    }
}

int Tape::getRunsCount() {
    return runsCount;
}

vector<Record *>* Tape::getNextRun() {
    runsCount -= 1;
    vector<Record*>* nextRun = DataManager::getNextRun();
    if (nextRun == nullptr) {
        return nullptr;
    }
    readingStart += nextRun->size() * RECORD_SIZE;
    return nextRun;
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