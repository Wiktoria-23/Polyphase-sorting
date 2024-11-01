#include <iostream>
#include "Program.h"
using namespace std;

//Co musi robić program?
// - SORTOWANIE POLIFAZOWE
// - taśma - plik dyskowy
// - wyświetlanie zawartości pliku przed i po posortowaniu
// - możliwość wyświetlenia pliku po każdej fazie sortowania (pewnie przerwanie i oczekiwanie na sygnał do kontynuacji sortowania)
// - zliczać fazy sortowania i liczbę operacji dyskowych
// dane odczytujemy seriami - dzielimy między trzy taśmy tak, by liczności odpowiadały liczbie fibonacciego
// potrzebna jest funckcja do obliczania następnej liczby fibonacciego - mośżemy przy taśmie zapisywać jej rozmiar


int main() {
	Program program = Program();
	program.runProgram();
	return 0;
}