#include "Program.h"
#include "DataManager.h"
#include "DiskPage.h"
#include "PolyphaseSorting.h"
#include "Record.h"

Program::Program() {
    dataManager = new DataManager("../src/data.dat");
    random_device random_device;
    numbersGenerator = new default_random_engine(random_device());
    numbersDistribution = new uniform_real_distribution<double>(0, 100);
    polyphaseSorting = new PolyphaseSorting(dataManager);
}

Program::~Program() {
    delete dataManager;
    delete numbersGenerator;
    delete numbersDistribution;
    delete polyphaseSorting;
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
    cout << "3. Wyswietl zawartosc pliku wejsciowego" << endl;
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
        generateData();
    }
    if (optionNumber == 2) {
        inputRecords();
    }
    if (optionNumber == 3) {
        cout << "Wyswietlanie danych wejsciowych:" << endl;
        dataManager->printFile();
    }
    if (optionNumber == 4) {
        polyphaseSorting->readDataFromFile();
        if (polyphaseSorting->calculateRunsAmount() == 0) {
            cout << "Plik jest pusty" << endl;
            return optionNumber;
        }
        dataManager->printFile();
        cout << "Czy chcesz wyswietlac tasmy po kazdej fazie sortowania? (1 - tak, 0 - nie)" << endl;
        int printTapes;
        cin >> printTapes;
        if (printTapes == 1) {
            cout << endl << "Tasmy przed sortowaniem" << endl;
            polyphaseSorting->printTapes();
        }
        while (true) {
            cout << endl << polyphaseSorting->getPhasesCount() + 1 << ". faza sortowania" << endl;
            polyphaseSorting->continueSorting();
            polyphaseSorting->increasePhasesCount();
            if (polyphaseSorting->isSorted()) {
                break;
            }
            if (printTapes == 1) {
                cout << endl << "Tasmy po scaleniu" << endl;
                polyphaseSorting->printTapes();
            }
        }
        cout << "Zakonczono sortowanie" << endl;
        cout << endl << "Plik po posortowaniu: " << endl;
        polyphaseSorting->printResultAndDeleteFiles();
        cout << "Sortowanie zostalo zakonczone po " << polyphaseSorting->getPhasesCount() << " fazach i wymagalo ";
        cout << polyphaseSorting->countDiskAccesses() << " dostepow do dysku." << endl;
        polyphaseSorting->reset();
    }
    return optionNumber;
}

void Program::generateData() {
    int recordsAmount;
    cout << "Wpisz liczbe rekordow do wygenerowania:";
    cin >> recordsAmount;
    Record* newRecord;
    dataManager->startInput();
    for (int i = 0; i < recordsAmount; i++) {
        newRecord = new Record(numbersGenerator, numbersDistribution);
        dataManager->writeRecordToFile(newRecord);
        delete newRecord;
    }
    dataManager->stopInput();
    polyphaseSorting->setNotSorted();
}

void Program::inputRecords() {
    int recordsAmount;
    double a, b, h;
    cout << "Podaj liczbe nowych rekordow:";
    cin >> recordsAmount;
    dataManager->startInput();
    for (int i = 0; i < recordsAmount; i++) {
        cout << "Rekord nr. " << i + 1 << endl;
        cout << "Podaj a:";
        cin >> a;
        cout << "Podaj b:";
        cin >> b;
        cout << "Podaj h:";
        cin >> h;
        dataManager->writeRecordToFile(new Record(a, b, h));
    }
    dataManager->stopInput();
}