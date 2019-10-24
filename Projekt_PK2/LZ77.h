#pragma once

#include <stdio.h>

#define un_eof 255

#define dictionaryBuffer_size_16kB 16            //16384B        16kB      
#define codeBuffer_size_255B 255              //               255B

struct buffersLZ77
{
	unsigned char* dictionaryBuffer;
	int dictionaryBuffer_size;

	unsigned char* codeBuffer;
	int codeBuffer_size;
};


void compressionLZ77(FILE*, FILE*, const int, const int);		//kompresja

void decompressionLZ77(FILE*, FILE*);	//dekompresja


void compare(struct buffersLZ77, short*, unsigned char*);	//szukanie najdluzszego ciagu

void bufferShift_compression(FILE*, struct buffersLZ77, const unsigned char);	//przesuniecie wartosci w tablicach przy kompresji

void bufferShift_decompression(struct buffersLZ77, const unsigned char, const unsigned char);	//przesuniecie wartosci w tablicach przy dekompresji