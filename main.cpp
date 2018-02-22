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

uint8_t	block1[16] = { 10, 25, 63, 100, 10, 12, 30, 125, 14, 101, 215, 13, 14, 12 , 200, 189 };

int main(int argc, char *argv[]) {
	
	BlockCompression BC4;
	BC4.compressBC4(block1);

	getchar();
	return 0;
}