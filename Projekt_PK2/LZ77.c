#include "LZ77.h"

#include <stdio.h>
#include <Windows.h>


void compressionLZ77(FILE *fileIn, FILE *fileOut, const int dictionaryBuffer_size_kB, const int codeBuffer_size)
{
	struct buffersLZ77 buffers;

	buffers.codeBuffer_size = codeBuffer_size;
	buffers.dictionaryBuffer_size = dictionaryBuffer_size_kB * 1024;	//zamiana z kB na B
	buffers.dictionaryBuffer = calloc(buffers.dictionaryBuffer_size, sizeof(unsigned char));
	buffers.codeBuffer = calloc(buffers.codeBuffer_size, sizeof(unsigned char));
	
	unsigned char size;
	short position;

	int iterations = 0;	//liczba przejsc petli do while
	fseek(fileIn, 0, SEEK_END);
	double file_size = ftell(fileIn);   //	czytanie wielkosci pliku
	fseek(fileIn, 0, SEEK_SET);


	fwrite(&dictionaryBuffer_size_kB, 1, 1, fileOut);		//zapis informacji o wielkosci buforow przy kompresji
	fwrite(&buffers.codeBuffer_size, 1, 1, fileOut);


	for (int i = 0; i < buffers.codeBuffer_size; i++)  //inicjalizacja tablicy wartoscia 255
		buffers.codeBuffer[i] = un_eof;

	buffers.codeBuffer[0] = getc(fileIn);
	fwrite(buffers.codeBuffer, 1, 1, fileOut);	// zapis 1 znaku

	for (int i = 0; i < buffers.dictionaryBuffer_size; i++)
		buffers.dictionaryBuffer[i] = buffers.codeBuffer[0];	//inicjalizacja tablicy 1 wartoscia z pliku

	for (int i = 0; i < buffers.codeBuffer_size; i++)
		if ((buffers.codeBuffer[i] = getc(fileIn)) == EOF)	//czytanie do bufora kodowania
			break;


	do
	{
		compare(buffers, &position, &size);		//szukanie identycznych ciagow
		fwrite(&position, 2, 1, fileOut);	//zapis wynikow (pozycja,rozmiar,nastepny znak)
		fwrite(&size, 1, 1, fileOut);
		fwrite(&buffers.codeBuffer[size], 1, 1, fileOut);

		size++;
		bufferShift_compression(fileIn, buffers, size);	//przesuniecie buforow

		if (!(iterations++ % 10000))
		{
			system("cls");
			printf("Ukonczono: %.0f%c", (((float)ftell(fileIn) / file_size) * 100), '%');   // wypisywanie postepu
		}

	} while (buffers.codeBuffer[0] != un_eof || !feof(fileIn));

	free(buffers.dictionaryBuffer);
	free(buffers.codeBuffer);

}

void bufferShift_compression(FILE*fileIn, struct buffersLZ77 buffers, const unsigned char size)
{
	for (int i = 0; i < buffers.dictionaryBuffer_size - size; i++)
		buffers.dictionaryBuffer[i] = buffers.dictionaryBuffer[i + size];
	for (int i = buffers.dictionaryBuffer_size - size; i < buffers.dictionaryBuffer_size; i++)
		buffers.dictionaryBuffer[i] = buffers.codeBuffer[i - buffers.dictionaryBuffer_size + size];

	for (int i = 0; i < buffers.codeBuffer_size - size; i++)
		buffers.codeBuffer[i] = buffers.codeBuffer[i + size];
	for (int i = buffers.codeBuffer_size - size; i < buffers.codeBuffer_size; i++)
		if ((buffers.codeBuffer[i] = getc(fileIn)) == un_eof)
			break;
}

void compare(struct buffersLZ77 buffers, short *position, unsigned char *maxSize)
{
	*maxSize = 0;	//najwiekszy znaleziony ciag
	*position = 0;	//pozycja poczatku najwiekszego ciagu
	unsigned char currentSize = 0;

	for (int i = buffers.dictionaryBuffer_size - 1; i >= 0; i--)
	{
		if (buffers.dictionaryBuffer[i] == buffers.codeBuffer[0])
		{
			currentSize++;
			while (currentSize + 1 < buffers.codeBuffer_size && i + currentSize < buffers.dictionaryBuffer_size &&
				buffers.dictionaryBuffer[i + currentSize] == buffers.codeBuffer[currentSize] && buffers.dictionaryBuffer[currentSize] != un_eof)
			{
				currentSize++;
			}
			if (currentSize > *maxSize)
			{
				*maxSize = currentSize;
				*position = i;
				if (*maxSize + 1 == buffers.codeBuffer_size)	//sprawdzanie czy znaleziono juz mozliwy najdluzszy ciag
					break;
			}
			currentSize = 0;
		}
	}
}

void decompressionLZ77(FILE *fileIn, FILE *fileOut)
{
	struct buffersLZ77 buffers;

	buffers.dictionaryBuffer_size = getc(fileIn) * 1024;	//czytanie wielkosci buforow
	buffers.codeBuffer_size = getc(fileIn);

	buffers.dictionaryBuffer = calloc(buffers.dictionaryBuffer_size, sizeof(unsigned char));
	buffers.codeBuffer = calloc(buffers.codeBuffer_size, sizeof(unsigned char));

	unsigned char sign = getc(fileIn);
	unsigned char size;
	short position;

	int iterations = 0;				//liczba przejsc petli do while
	fseek(fileIn, 0, SEEK_END);
	double file_size = ftell(fileIn);   //	czytanie wielkosci pliku
	fseek(fileIn, 3, SEEK_SET);


	for (int i = 0; i < buffers.dictionaryBuffer_size; i++)	//inicjalizacja tablicy 1 znakiem
		buffers.dictionaryBuffer[i] = sign;
	fwrite(&sign, 1, 1, fileOut);

	do
	{
		fread(&position, 2, 1, fileIn);	//odczytywanie kodu (pozycja,rozmiar,nastepny znak)
		size = getc(fileIn);
		sign = getc(fileIn);

		for (int i = 0; i < size; i++)
		{
			fwrite(&buffers.dictionaryBuffer[i + position], 1, 1, fileOut);
			buffers.codeBuffer[i] = buffers.dictionaryBuffer[i + position];
		}
		if (sign == un_eof)	//warunek musi byc przed wypisaniem ostatniego znaku i po wypisaniu ostatniej serii z bufora
			break;
		fwrite(&sign, 1, 1, fileOut);

		size++;
		bufferShift_decompression(buffers, size, sign);	//przesuwanie tablic


		if (!(iterations++ % 10000))
		{
			system("cls");
			printf("Ukonczono: %.0f%c", (((float)ftell(fileIn) / file_size) * 100), '%');   // wypisywanie postepu
		}

	} while (!feof(fileIn));

	free(buffers.dictionaryBuffer);
	free(buffers.codeBuffer);

}

void bufferShift_decompression(struct buffersLZ77 buffers, const unsigned char size, const unsigned char sign)
{
	for (int i = 0; i < buffers.dictionaryBuffer_size - size; i++)
		buffers.dictionaryBuffer[i] = buffers.dictionaryBuffer[i + size];
	for (int i = buffers.dictionaryBuffer_size - size; i < buffers.dictionaryBuffer_size - 1; i++)
		buffers.dictionaryBuffer[i] = buffers.codeBuffer[i - buffers.dictionaryBuffer_size + size];
	buffers.dictionaryBuffer[buffers.dictionaryBuffer_size - 1] = sign;
}