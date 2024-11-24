#ifndef RECORD_H
#define RECORD_H
#include <random>
using namespace std;

class Record {
public:
    Record();
    Record(default_random_engine* generator, uniform_real_distribution<double>* distribution);
    Record(double a, double b, double h);
    ~Record();
    double calculateField() const;
    double getA();
    double getB();
    double getH();
    void setA(double a);
    void setB(double b);
    void setH(double h);
private:
    double a;
    double b;
    double h;
};



#endif //RECORD_H
