#ifndef PROGRAM_H
#define PROGRAM_H
#define RECORD_SIZE (3*sizeof(double))
#define DISK_PAGE_SIZE (100*RECORD_SIZE)
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>
#include <random>
using namespace std;

class DataManager;
class PolyphaseSorting;

class Program {
public:
    Program();
    ~Program();
    void runProgram();
private:
    default_random_engine* numbersGenerator;
    uniform_real_distribution<double>* numbersDistribution;
    DataManager* dataManager;
    PolyphaseSorting* polyphaseSorting;
    void printMenu();
    int chooseOption();
    void generateData();
    void inputRecords();
};



#endif //PROGRAM_H
