#include "DataManager.h"

#include "Program.h"
#include "Record.h"

DataManager::DataManager() {
    // próbujemy otworzyć plik z danymi, jeżeli plik nie zostaje poprawnie otwarty (nie istnieje) tworzymy nowy
    this->fileStream.open(this->dataPath, std::ios::binary | std::ios::out | std::ios::in);
    if (!fileStream) {
        cout << "Nie udalo sie otworzyc pliku z danymi - tworzymy nowy plik" << endl;
        this->fileStream.clear();
        this->fileStream.open(this->dataPath, std::ios::binary | std::ios::out);
    }
    fileStream.close();
}

DataManager::~DataManager() {
    if (this->fileStream.is_open()) {
        this->fileStream.close();
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
}

void DataManager::stopInput() {
    this->fileStream.close();
}

void DataManager::printFile() {
    this->fileStream.open(this->dataPath, std::ios::binary | std::ios::in);
    cout << "Wyswietlanie zawartosci pliku:" << endl;
    Record* record;
    int counter = 1;
    while ((record = this->readRecordFromFile()) != nullptr) {
        cout << counter << ". " << record->getA() << " " << record->getB() << " " << record->getH() << endl;
        counter++;
        delete record;
    }
    this->fileStream.close();
}

Record *DataManager::readRecordFromFile() {
    double record[3];
    if (this->fileStream.read(reinterpret_cast<char*>(record), RECORD_SIZE)) {
        return new Record(record[0], record[1], record[2]);
    }
    return nullptr;
}

