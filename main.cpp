/*
*	Author: Augusto Gomez
*   Description: Block Compression algorithm (BC4)
*/

/**
\file     main.cpp
\brief    Main function for running the papplication
*/

#include "BlockCompression.h"
#include <stdio.h>

uint8_t	block1[16] = { 101, 101, 102, 103, 104, 104, 105, 105, 106, 107, 107, 108, 109, 110 , 110, 111 };

int main(int argc, char *argv[]) {

	BlockCompression BC4;

	printf("Compress Block\n");
	uint64_t compressBlock = BC4.compressBC4(block1);

	printf("\nDecompress Block\n");
	uint8_t* decompressBlock = BC4.decompressBC4(compressBlock);

	getchar();
	return 0;
}