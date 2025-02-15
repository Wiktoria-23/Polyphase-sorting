#include "DiskPage.h"
#include "Record.h"
#include "Program.h"

DiskPage::DiskPage() {
    capacity = DISK_PAGE_SIZE/RECORD_SIZE;
    records = new vector<Record*>;
    full = false;
    index = 0;
}

DiskPage::~DiskPage() {
    //all records are deleted before
    delete records;
}

vector<Record*>* DiskPage::getRecords() {
    return records;
}

void DiskPage::clear() {
    records->clear();
    full = false;
    index = 0;
}

void DiskPage::writeRecordToDiskPage(Record *record) {
    getRecords()->push_back(record);
    if (records->size() == DISK_PAGE_SIZE/RECORD_SIZE) {
        full = true;
    }
}

bool DiskPage::isFull() {
    return full;
}

void DiskPage::increaseIndex() {
    index++;
}

int DiskPage::getIndex() {
    return index;
}

void DiskPage::deleteAllRecords() {
    for (int i = 0; i < records->size(); i++) {
        if (records->at(i) != nullptr) {
            delete records->at(i);
        }
    }
}

Record *DiskPage::getRecordFromDiskPage(int index) {
    return records->at(index);
}

