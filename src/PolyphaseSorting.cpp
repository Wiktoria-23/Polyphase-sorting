#include "PolyphaseSorting.h"
#include "Tape.h"
#include "DataManager.h"
#include "DiskPage.h"
#include "Record.h"

PolyphaseSorting::PolyphaseSorting(DataManager* dataManager) {
    this->tape1 = new Tape("../src/tape1.dat");
    this->tape2 = new Tape("../src/tape2.dat");
    this->tape3 = new Tape("../src/tape3.dat");
    this->dataManager = dataManager;
    this->sorted = false;
    this->phasesCount = 0;
}

PolyphaseSorting::~PolyphaseSorting() {
    delete tape1;
    delete tape2;
    delete tape3;
}

void PolyphaseSorting::readDataFromFile() {
    // dystrybucja danych pomiędzy taśmy
    this->dataManager->startReadingData();
    this->dataManager->readNextDiskPageFromFile();
    vector<Record*>* run;
    // ustawiamy początkowe wartości na maksymalnego double by zaznaczyć że trzeba zwiększyć liczbę rekordów
    double lastElementValue1 = numeric_limits<double>::max();
    double lastElementValue2 = numeric_limits<double>::max();
    tape1->startInput();
    tape2->startInput();
    if (dataManager->getCurrentDiskPage()->getRecords()->empty()) {
        this->sorted = true;
        this->dataManager->stopReadingData();
        tape1->stopInput();
        tape2->stopInput();
        return;
    }
    do {
        run = this->dataManager->getNextRun();
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
    } while (!this->dataManager->isFileRead() || this->dataManager->getCurrentDiskPage()->getIndex() < this->dataManager->getCurrentDiskPage()->getRecords()->size());
    this->getReadyToSort();
    this->dataManager->stopReadingData();
    this->tape1->stopInput();
    this->tape2->stopInput();
    if (this->tape1->getRunsCount() == 1 && this->tape2->getRunsCount() == 0) {
        this->sorted = true;
    }
}

void PolyphaseSorting::continueSorting() {
    // pusta taśma to będzie zawsze taśma 3, po wykonaniu scalania podmieniamy wskaźniki tak, by wszystko się zgadzało
    // dłuższa taśma to taśma nr 2
    // krótsza taśma to taśma nr 1
    // wykonujemy jedną fazę sortowania
    // następnie mergujemy wszystkie rekordy z mniejszej taśmy z odpowiednią liczbą rekordów z większej taśmy
    // wynik zostaje zapisany na pustej taśmie
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
    // po sortowaniu pustą taśmą będzie taśma nr 1
    // taśmą dłuższą będzie taśma nr 3
    // taśmą krótszą będzie taśma nr 2
    // PRZY PODMIANIE TAŚM JESZCZE TRZEBA ZADBAĆ O TO, BY ICH POJEMNOŚCI SIĘ ZGADZALY Z LICZBĄ REKORDÓW
    Tape* tmp = tape3;
    tape3 = tape1;
    tape1 = tape2;
    tape2 = tmp;
    remove(this->tape3->getFilename());
    this->tape3->createNewFile();
    if (calculateRunsAmount() == 1) {
        this->sorted = true;
    }
}

bool PolyphaseSorting::isSorted() {
    return this->sorted;
}

void PolyphaseSorting::printTapes() {
    cout << endl << "Tasma 1" << endl;
    this->tape1->printFile();
    cout << endl << "Tasma 2" << endl;
    this->tape2->printFile();
    cout << endl << "Tasma 3" << endl;
    this->tape3->printFile();
}

void PolyphaseSorting::increasePhasesCount() {
    this->phasesCount++;
}

void PolyphaseSorting::getReadyToSort() {
    // sprawdzamy czy taśmy maja odpowiednią wielkość
    // pustą serię którą dodajemy do taśmy oznaczamy za pomocą rekordu z ujemnym polem
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
        else if (run1->empty() || (!run2->empty() && run2->front()->calculateField() <= run1->front()->calculateField())) {
            if (run2->front()->calculateField() > 0) {
                result->push_back(run2->front());
            }
            run2->erase(run2->begin());
        }
    }
    return result;
}

int PolyphaseSorting::getPhasesCount() {
    return this->phasesCount;
}

int PolyphaseSorting::countDiskAccesses() {
    return this->tape1->getDiskAccessCounter() + this->tape2->getDiskAccessCounter() + this->tape3->getDiskAccessCounter();
}

int PolyphaseSorting::calculateRunsAmount() {
    return (this->tape1->getRunsCount() + this->tape2->getRunsCount() + this->tape3->getRunsCount());
}

void PolyphaseSorting::printResultAndDeleteFiles() {
    if (this->tape2->getRunsCount() != 0) {
        this->tape2->printFile();
    }
    else {
        this->tape1->printFile();
    }
    string path = this->dataManager->getFilename();
    remove(this->dataManager->getFilename());
    if (this->tape2->getRunsCount() > 0) {
        rename(this->tape2->getFilename(), path.c_str());
        remove(this->tape1->getFilename());
    }
    else {
        rename(this->tape1->getFilename(), path.c_str());
        remove(this->tape2->getFilename());
    }
    remove(this->tape3->getFilename());
    this->tape1->createNewFile();
    this->tape2->createNewFile();
    this->tape3->createNewFile();
}

void PolyphaseSorting::setNotSorted() {
    this->sorted = false;
}

void PolyphaseSorting::reset() {
    this->sorted = false;
    this->phasesCount = 0;
    string path = this->dataManager->getFilename();
    delete this->dataManager;
    this->dataManager = new DataManager(path);
    this->tape1->reset();
    this->tape2->reset();
    this->tape3->reset();
}