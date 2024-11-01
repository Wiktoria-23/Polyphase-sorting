#include "Program.h"

Program::Program() {
    this->dataManager = new DataManager();
    random_device random_device;
    this->numbersGenerator = new default_random_engine(random_device());
    this->numbersDistribution = new uniform_real_distribution<double>(0, 100);
}

Program::~Program() {
    delete this->dataManager;
    delete this->numbersGenerator;
    delete this->numbersDistribution;
}

void Program::runProgram() {
    while (true) {
        printMenu();
        if (chooseOption() == 5) {
            break;
        }
    }
}

void Program::printMenu() {
    cout << "Wpisz numer operacji do wykonania:" << endl;
    cout << "1. Generuj rekordy" << endl;
    cout << "2. Wpisywanie rekordow" << endl;
    cout << "3. Wyswietl zawartosc pliku" << endl;
    cout << "4. Wykonaj sortowanie" << endl;
    cout << "5. Wyjdz" << endl;
}

int Program::chooseOption() {
    int optionNumber;
    cin >> optionNumber;
    if (optionNumber < 1 || optionNumber > 5) {
        cout << "Wprowadzono bledna opcje" << endl;
        return 0;
    }
    if (optionNumber == 1) {
        this->generateData();
    }
    if (optionNumber == 2) {
        this->inputRecords();
    }
    if (optionNumber == 3) {
        this->dataManager->printFile();
    }
    if (optionNumber == 4) {
        this->sortFile();
    }
    return optionNumber;
}

void Program::generateData() {
    int recordsAmount;
    cout << "Wpisz liczbe rekordow do wygenerowania:";
    cin >> recordsAmount;
    Record* newRecord;
    this->dataManager->startInput();
    for (int i = 0; i < recordsAmount; i++) {
        newRecord = new Record(this->numbersGenerator, this->numbersDistribution);
        this->dataManager->writeRecordToFile(newRecord);
        delete newRecord;
    }
    this->dataManager->stopInput();
}

void Program::inputRecords() {
    int recordsAmount;
    double a, b, h;
    cout << "Podaj liczbe nowych rekordow:";
    cin >> recordsAmount;
    this->dataManager->startInput();
    for (int i = 0; i < recordsAmount; i++) {
        cout << "Rekord nr. " << i + 1 << endl;
        cout << "Podaj a:";
        cin >> a;
        cout << "Podaj b:";
        cin >> b;
        cout << "Podaj h:";
        cin >> h;
        this->dataManager->writeRecordToFile(new Record(a, b, h));
    }
    this->dataManager->stopInput();
}

void Program::sortFile() {
    // tutaj trzeba napisać sortowanie - ostatecznie chyba nie będzie to w tym pliku
}