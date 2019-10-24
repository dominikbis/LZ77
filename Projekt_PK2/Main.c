#include <stdio.h>
#include <Windows.h>
#include <time.h>

#include "LZ77.h"

#define ascii_1 49
#define ascii_2 50

enum state
{
	compression = ascii_1,
	decompression = ascii_2,
};


int main()
{
	FILE*fileIn;
	FILE*fileOut;

	int dictionaryBuffer_size_kB;
	int codeBuffer_size;

	char* file[255];

	int timeStart;		//zmienne do mierzenia czasu
	int timeStop;

	char choice;
	do
	{
		printf("Program do slownikowej kompresji danych algorytmem LZ77\n\n");
		printf("Wybierz dzialanie:\n[1] - kompresja\n[2] - dekompresja\n");
		choice = getch();
		system("cls");
	} while (choice != ascii_1 && choice != ascii_2);

	do
	{
		printf("Podaj nazwe pliku wejsciowego wraz z rozszerzeniem(np. text.txt): ");
		scanf("%s", file);
	} while ((fileIn = fopen(file, "rb")) == NULL);

	printf("Podaj nazwe pliku wyjsciowego wraz z rozszerzeniem(np. text.txt): ");
	scanf("%s", file);
	fileOut = fopen(file, "wb");

	if (choice == compression)
	{
		do
		{
			system("cls");
			printf("wielkosc bufora ma wplyw na stopien kompresji oraz czas realizacji zadania\n\n");
			printf("Podaj rozmiar bufora slownikowego w kB(od 1 do 16): ");
			scanf("%d", &dictionaryBuffer_size_kB);
		} while (dictionaryBuffer_size_kB < 1 || dictionaryBuffer_size_kB > 16);		//1-16
		do
		{
			system("cls");
			printf("Podaj rozmiar bufora kodowania w B(od 10 do 255): ");
			scanf("%d", &codeBuffer_size);
		} while (codeBuffer_size < 10 || codeBuffer_size > 255);	//10-255

		timeStart = clock();
		compressionLZ77(fileIn, fileOut, dictionaryBuffer_size_kB, codeBuffer_size);	//kompresja
		timeStop = clock();

		system("cls");
		printf("Stopien kompresii: %f\n", ((float)ftell(fileIn)/(float)ftell(fileOut)));
	}
	else
	{
		timeStart = clock();
		decompressionLZ77(fileIn, fileOut);		//dekompresja
		timeStop = clock();
		system("cls");
	}
	printf("Czas trwania %f s\n\n", (float)(timeStop - timeStart) / 1000);
	printf("Nacijnij dowolny przycisk aby zakonczyc");
	getch();
	
}