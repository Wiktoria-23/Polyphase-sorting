#include "Record.h"

Record::Record() {
    a = 0;
    b = 0;
    h = 0;
}

Record::Record(default_random_engine* generator, uniform_real_distribution<double>* distribution) {
    a = (*distribution)(*generator);
    b = (*distribution)(*generator);
    h = (*distribution)(*generator);
}

Record::Record(double newA, double newB, double newH) {
    a = newA;
    b = newB;
    h = newH;
}

Record::~Record() = default;

double Record::calculateField() const {
    return (a + b) * h / 2;
}

double Record::getA() {
    return a;
}

double Record::getB() {
    return b;
}

double Record::getH() {
    return h;
}