#include <iostream>
#include "Program.h"
using namespace std;

//Co musi robić program?
// - generowanie rekordów
// - wpisywanie rekordów z klawiatury 
// GENEROWANIE I WPISYWANIE REKORDÓW TO OSOBNA WARSTWA W PROGRAMIE - powinna udostępniać odczyt i zapis pojedynczego rekordu
// - używa plików binarnych LUB TXT (?) do zapisu informacji
// - JAKIŚ TYP SORTOWANIA JESZCZE NIE WIEM JAKI
// - DANE: dwie długości podstaw trapezu oraz jego wysokość - sortowanie według pola (jakaś funkcja licząca pole)
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