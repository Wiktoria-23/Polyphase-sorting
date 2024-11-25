#include "Tape.h"
#include "DataManager.h"
#include "DiskPage.h"
#include "Program.h"

Tape::Tape(string dataPath): DataManager(dataPath) {
    this->currentCapacity = 1;
    this->runsCount = 0;
    this->readingStart = 0;
}

Tape::~Tape() {
    remove(this->dataPath.c_str());
}

void Tape::increaseCapacity(int toAdd) {
    this->currentCapacity += toAdd;
}

int Tape::getCapacity() {
    return currentCapacity;
}

void Tape::writeRunToDiskPage(vector<Record*>* run, bool continueRun) {
    for (int i = 0; i < run->size(); i++) {
        // jeżeli taśma jest pełna to zapisujemy stronę dyskową na taśmę
        if (this->currentDiskPage->isFull()) {
            this->writeDiskPageToFile();
        }
        this->currentDiskPage->writeRecordToDiskPage(run->at(i));
    }
    if (!continueRun) {
        this->runsCount++;
    }
}

int Tape::getRunsCount() {
    return this->runsCount;
}

vector<Record *>* Tape::getNextRun() {
    this->runsCount -= 1;
    vector<Record*>* nextRun = DataManager::getNextRun();
    if (nextRun == nullptr) {
        return nullptr;
    }
    this->readingStart += nextRun->size() * RECORD_SIZE;
    return nextRun;
}

void Tape::startReadingData() {
    DataManager::startReadingData();
    fileStream.seekg(this->readingStart);
}

void Tape::startInput() {
    DataManager::startInput();
    fileStream.seekp(this->readingStart);
}

void Tape::printFile() {
    this->startReadingData();
    fileStream.seekg(this->readingStart);
    this->printRecords();
    this->stopReadingData();
}

void Tape::setCapacity(int newCapacity) {
    this->currentCapacity = newCapacity;
}

void Tape::createNewFile() {
    DataManager::createNewFile();
    this->readingStart = 0;
}

void Tape::reset() {
    this->diskAccessCounter = 0;
    this->runsCount = 0;
}