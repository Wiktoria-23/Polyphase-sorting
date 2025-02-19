#include "DataManager.h"
#include "Tape.h"
#include <utility>
#include "Program.h"
#include "Record.h"
#include "DiskPage.h"

DataManager::DataManager(string newDataPath) {
    diskAccessCounter = 0;
    dataPath = std::move(newDataPath);
    DataManager::createNewFile();
    currentDiskPage = nullptr;
}

DataManager::~DataManager() {
    if (fileStream.is_open()) {
        fileStream.close();
    }
    if (currentDiskPage != nullptr) {
        delete currentDiskPage;
    }
}

void DataManager::writeRecordToFile(Record* record) {
    double dataToWrite[3];
    dataToWrite[0] = record->getA();
    dataToWrite[1] = record->getB();
    dataToWrite[2] = record->getH();
    fileStream.write(reinterpret_cast<char*>(dataToWrite), RECORD_SIZE);
}

void DataManager::startInput() {
    fileStream.open(dataPath, std::ios::binary | std::ios::app);
    currentDiskPage = new DiskPage();
    continueReadingData = true;
}

void DataManager::stopInput() {
    writeDiskPageToFile();
    delete currentDiskPage;
    currentDiskPage = nullptr;
    fileStream.close();
}

void DataManager::printFile() {
    startReadingData();
    printRecords();
    stopReadingData();
}

Record* DataManager::readRecordFromFile() {
    double record[3];
    if (this->fileStream.read(reinterpret_cast<char*>(record), RECORD_SIZE)) {
        Record* newRecord =  new Record(record[0], record[1], record[2]);
        return newRecord;
    }
    return nullptr;
}

void DataManager::readNextDiskPageFromFile() {
    double records[3 * DISK_PAGE_SIZE/RECORD_SIZE];
    currentDiskPage->clear();
    fileStream.read(reinterpret_cast<char*>(records), DISK_PAGE_SIZE);
    int recordsRead = fileStream.gcount() / RECORD_SIZE;

    for (int i = 0; i < recordsRead; i++) {
        currentDiskPage->getRecords()->push_back(
            new Record(records[(3 * i)], records[(3 * i) + 1], records[(3 * i) + 2])
            );
    }
    if (recordsRead < DISK_PAGE_SIZE/RECORD_SIZE) {
        continueReadingData = false;
    }
    diskAccessCounter += 1;
}

void DataManager::startReadingData() {
    continueReadingData = true;
    currentDiskPage = new DiskPage();
    currentDiskPage->clear();
    fileStream.open(dataPath, std::ios::binary | std::ios::in | std::ios::out);
}

void DataManager::stopReadingData() {
    continueReadingData = false;
    delete currentDiskPage;
    currentDiskPage = nullptr;
    fileStream.close();
}

bool DataManager::isFileRead() {
    return !continueReadingData;
}

vector<Record*>* DataManager::getNextRun() {
    if (currentDiskPage->getIndex() >= currentDiskPage->getRecords()->size() && continueReadingData) {
        readNextDiskPageFromFile();
    }
    if (!continueReadingData && currentDiskPage->getRecords()->empty()) {
        return nullptr;
    }
    vector<Record*>* run = new vector<Record*>;
    Record* currentRecord = currentDiskPage->getRecords()->at(currentDiskPage->getIndex());
    if (currentRecord->calculateField() < 0) {
        // rekord z ujemnym polem - koniec serii
        run->push_back(currentRecord);
        currentDiskPage->increaseIndex();
        return run;
    }
    double previousValue = -1;
    while (previousValue <= currentRecord->calculateField()) {
        run->push_back(currentRecord);
        currentDiskPage->increaseIndex();
        if (currentDiskPage->getIndex() >= currentDiskPage->getRecords()->size()) {
            if (continueReadingData) {
                readNextDiskPageFromFile();
            }
            else {
                break;
            }
        }
        previousValue = currentRecord->calculateField();
        if (currentDiskPage->getRecords()->size() != 0) {
            currentRecord = currentDiskPage->getRecordFromDiskPage(this->currentDiskPage->getIndex());
        }
        else {
            break;
        }
    }
    return run;
}

void DataManager::writeDiskPageToFile() {
    int recordsAmount = currentDiskPage->getRecords()->size();
    double dataToWrite[3 * recordsAmount];
    for (int i = 0; i < recordsAmount; i+=1) {
        dataToWrite[(3 * i)] = currentDiskPage->getRecordFromDiskPage(i)->getA();
        dataToWrite[(3 * i) + 1] = currentDiskPage->getRecordFromDiskPage(i)->getB();
        dataToWrite[(3 * i) + 2] = currentDiskPage->getRecordFromDiskPage(i)->getH();
    }
    fileStream.write(reinterpret_cast<char*>(dataToWrite), RECORD_SIZE * recordsAmount);
    currentDiskPage->deleteAllRecords();
    currentDiskPage->clear();
    diskAccessCounter += 1;
}

int DataManager::getDiskAccessCounter() {
    return diskAccessCounter;
}

DiskPage* DataManager::getCurrentDiskPage() {
    return currentDiskPage;
}

void DataManager::printRecords() {
    int counter = 1;
    cout << "(numer rekordu, a, b, h, pole)" << endl;
    Record* record = nullptr;
    readNextDiskPageFromFile();
    while (true) {
        if (currentDiskPage->getIndex() >= currentDiskPage->getRecords()->size()) {
            readNextDiskPageFromFile();
            diskAccessCounter -= 1;  // access to file is not counted when printing
        }
        if (currentDiskPage->getRecords()->size() > 0) {
            record = currentDiskPage->getRecordFromDiskPage(currentDiskPage->getIndex());
            currentDiskPage->increaseIndex();
            cout << counter << ". " << record->getA() << " " << record->getB() << " " << record->getH() << " ";
            cout << record->calculateField() << endl;
            counter++;
        }
        else {
            record = nullptr;
        }
        if (record == nullptr && counter == 1) {
            cout << "Plik jest pusty" << endl;
            break;
        }
        if (record == nullptr
            || (!continueReadingData && currentDiskPage->getIndex() >= currentDiskPage->getRecords()->size())) {
            break;
        }
    }
}

const char* DataManager::getFilename() {
    return dataPath.c_str();
}

void DataManager::createNewFile() {
    fileStream.open(dataPath, std::ios::binary | std::ios::out);
    fileStream.close();
}

void DataManager::increaseReadingStart() {
    readingStart += RECORD_SIZE;
}
