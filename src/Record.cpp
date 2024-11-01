#include "Record.h"

Record::Record(default_random_engine* generator, uniform_real_distribution<double>* distribution) {
    this->a = (*distribution)(*generator);
    this->b = (*distribution)(*generator);
    this->h = (*distribution)(*generator);
}

Record::Record(double a, double b, double h) {
    this->a = a;
    this->b = b;
    this->h = h;
}

Record::~Record() {

}

double Record::calculateField() const {
    return (this->a + this->b) * this->h / 2;
}

double Record::getA() {
    return this->a;
}

double Record::getB() {
    return this->b;
}

double Record::getH() {
    return this->h;
}