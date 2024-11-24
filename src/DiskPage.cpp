#include "DiskPage.h"
#include "Record.h"
#include "Program.h"

DiskPage::DiskPage() {
    this->capacity = DISK_PAGE_SIZE/RECORD_SIZE;
    this->records = new vector<Record*>;
    this->full = false;
    this->index = 0;
}

DiskPage::~DiskPage() {
    // rekordy są usuwane zawsze odpowiednio wcześniej
    delete records;
}

vector<Record*>* DiskPage::getRecords() {
    return this->records;
}

void DiskPage::clear() {
    records->clear();
    this->full = false;
    this->index = 0;
}

void DiskPage::writeRecordToDiskPage(Record *record) {
    this->getRecords()->push_back(record);
    if (this->records->size() == DISK_PAGE_SIZE/RECORD_SIZE) {
        this->full = true;
    }
}

bool DiskPage::isFull() {
    return this->full;
}

void DiskPage::increaseIndex() {
    this->index++;
}

int DiskPage::getIndex() {
    return this->index;
}

void DiskPage::deleteAllRecords() {
    for (int i = 0; i < this->records->size(); i++) {
        if (this->records->at(i) != nullptr) {
            delete this->records->at(i);
        }
    }
}

Record *DiskPage::getRecordFromDiskPage(int index) {
    return this->records->at(index);
}

