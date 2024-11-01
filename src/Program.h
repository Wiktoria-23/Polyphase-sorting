#ifndef PROGRAM_H
#define PROGRAM_H
#define RECORD_SIZE (3*sizeof(double))
#include <string>
#include <iostream>
#include <fstream>
#include <random>
#include "DataManager.h"
using namespace std;


class Program {
public:
    Program();
    ~Program();
    void runProgram();
private:
    DataManager* dataManager;
    default_random_engine* numbersGenerator;
    uniform_real_distribution<double>* numbersDistribution;
    void printMenu();
    int chooseOption();
    void generateData();
    void inputRecords();
    void sortFile();
};



#endif //PROGRAM_H
