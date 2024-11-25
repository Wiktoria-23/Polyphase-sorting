#include "DataManager.h"
#include "Program.h"
#include "Record.h"
#include "DiskPage.h"

DataManager::DataManager(string dataPath) {
    // próbujemy otworzyć plik z danymi, jeżeli plik nie zostaje poprawnie otwarty (nie istnieje) tworzymy nowy
    this->diskAccessCounter = 0;
    this->dataPath = dataPath;
    this->createNewFile();
    this->currentDiskPage = nullptr;
}

DataManager::~DataManager() {
    if (this->fileStream.is_open()) {
        this->fileStream.close();
    }
    if (this->currentDiskPage != nullptr) {
        delete this->currentDiskPage;
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
    this->fileStream.open(this->dataPath, std::ios::binary | std::ios::app);
    this->currentDiskPage = new DiskPage();
    this->continueReadingData = true;
}

void DataManager::stopInput() {
    this->writeDiskPageToFile();
    delete this->currentDiskPage;
    this->currentDiskPage = nullptr;
    this->fileStream.close();
}

void DataManager::printFile() {
    this->startReadingData();
    this->printRecords();
    this->stopReadingData();
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
    this->currentDiskPage->clear();
    this->fileStream.read(reinterpret_cast<char*>(records), DISK_PAGE_SIZE);
    int recordsRead = this->fileStream.gcount() / RECORD_SIZE;

    for (int i = 0; i < recordsRead; i++) {
        this->currentDiskPage->getRecords()->push_back(new Record(records[(3 * i)], records[(3 * i) + 1], records[(3 * i) + 2]));
    }
    if (recordsRead < DISK_PAGE_SIZE/RECORD_SIZE) {
        this->continueReadingData = false;
    }
    this->diskAccessCounter += 1;
}

void DataManager::startReadingData() {
    this->continueReadingData = true;
    this->currentDiskPage = new DiskPage();
    this->currentDiskPage->clear();
    this->fileStream.open(this->dataPath, std::ios::binary | std::ios::in | std::ios::out);
}

void DataManager::stopReadingData() {
    this->continueReadingData = false;
    delete this->currentDiskPage;
    this->currentDiskPage = nullptr;
    this->fileStream.close();
}

bool DataManager::isFileRead() {
    return !this->continueReadingData;
}

vector<Record*>* DataManager::getNextRun() {
    if (this->currentDiskPage->getIndex() >= this->currentDiskPage->getRecords()->size() && continueReadingData) {
        this->readNextDiskPageFromFile();
    }
    if (!continueReadingData && this->currentDiskPage->getRecords()->empty()) {
        return nullptr;
    }
    vector<Record*>* run = new vector<Record*>;
    Record* currentRecord = this->currentDiskPage->getRecords()->at(this->currentDiskPage->getIndex());
    if (currentRecord->calculateField() < 0) {
        // rekord z ujemnym polem - koniec serii
        run->push_back(currentRecord);
        this->currentDiskPage->increaseIndex();
        return run;
    }
    double previousValue = -1;
    while (previousValue <= currentRecord->calculateField()) {
        run->push_back(currentRecord);
        this->currentDiskPage->increaseIndex();
        if (this->currentDiskPage->getIndex() >= this->currentDiskPage->getRecords()->size()) {
            if (continueReadingData) {
                this->readNextDiskPageFromFile();
            }
            else {
                break;
            }
        }
        previousValue = currentRecord->calculateField();
        if (this->currentDiskPage->getRecords()->size() != 0) {
            currentRecord = this->currentDiskPage->getRecordFromDiskPage(this->currentDiskPage->getIndex());
        }
        else {
            break;
        }
    }
    return run;
}

void DataManager::writeDiskPageToFile() {
    int recordsAmount = this->currentDiskPage->getRecords()->size();
    double dataToWrite[3 * recordsAmount];
    for (int i = 0; i < recordsAmount; i+=1) {
        dataToWrite[(3 * i)] = this->currentDiskPage->getRecordFromDiskPage(i)->getA();
        dataToWrite[(3 * i) + 1] = this->currentDiskPage->getRecordFromDiskPage(i)->getB();
        dataToWrite[(3 * i) + 2] = this->currentDiskPage->getRecordFromDiskPage(i)->getH();
    }
    fileStream.write(reinterpret_cast<char*>(dataToWrite), RECORD_SIZE * recordsAmount);
    this->currentDiskPage->deleteAllRecords();
    this->currentDiskPage->clear();
    this->diskAccessCounter += 1;
}

int DataManager::getDiskAccessCounter() {
    return this->diskAccessCounter;
}

DiskPage* DataManager::getCurrentDiskPage() {
    return this->currentDiskPage;
}

void DataManager::printRecords() {
    int counter = 1;
    cout << "(numer rekordu, a, b, h, pole)" << endl;
    Record* record = nullptr;
    this->readNextDiskPageFromFile();
    while (true) {
        if (this->currentDiskPage->getIndex() >= this->currentDiskPage->getRecords()->size()) {
            this->readNextDiskPageFromFile();
            this->diskAccessCounter -= 1;  // nie zliczamy dostępów do dysku przy wyświetlaniu pliku
        }
        if (this->currentDiskPage->getRecords()->size() > 0) {
            record = this->currentDiskPage->getRecordFromDiskPage(this->currentDiskPage->getIndex());
            this->currentDiskPage->increaseIndex();
        }
        else {
            record = nullptr;
        }
        if (record == nullptr && counter == 1) {
            cout << "Plik jest pusty" << endl;
            break;
        }
        cout << counter << ". " << record->getA() << " " << record->getB() << " " << record->getH() << " " << record->calculateField() << endl;
        counter++;
        if (record == nullptr || (!this->continueReadingData && this->currentDiskPage->getIndex() >= this->currentDiskPage->getRecords()->size())) {
            break;
        }
    }
    /*Record* record;
    int counter = 1;
    cout << "(numer rekordu, a, b, h, pole)" << endl;
    do {
        record = this->readRecordFromFile();
        if (record == nullptr && counter == 1) {
            cout << "Plik jest pusty" << endl;
            break;
        }
        if (record == nullptr) {
            break;
        }
        cout << counter << ". " << record->getA() << " " << record->getB() << " " << record->getH() << " " << record->calculateField() << endl;
        counter++;
        delete record;
    } while (true);*/
}

const char* DataManager::getFilename() {
    return this->dataPath.c_str();
}

void DataManager::createNewFile() {
    this->fileStream.open(this->dataPath, std::ios::binary | std::ios::in | std::ios::out);
    if (!fileStream) {
        this->fileStream.clear();
        this->fileStream.open(this->dataPath, std::ios::binary | std::ios::out);
    }
    fileStream.close();
}
