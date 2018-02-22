/*
*	Author: Augusto Gomez
*   Description: Block Compression algorithm (BC4)
*/

/**
\file     BlockCompression.h
\brief    compress an b4x4 block with BC4 algorithm
*/
#include "BlockCompression.h"
#include <limits>

#include <bitset>
#include <iostream>

/** Block Compression Constructor
\param    block    4x4 block for compressing
*/
BlockCompression::BlockCompression(){

}

/** Block Compression Destructor
*/
BlockCompression::~BlockCompression() {
}

/** Compress a 4x4 block using BC4 algorithm
\param    block    4x4 block for compressing
*/
uint64_t BlockCompression::compressBC4(uint8_t block[16]) {

	uint64_t	bc4 = 0;

	uint8_t		red_0;
	uint8_t		red_1;
		
	float		lookupTable[8];			///< reference color table 
	uint8_t		indexTable[16];			///< Index of the reference colors
	
	/// Calculate de value of red_0 and red_1
	calculateEndpoints(block, red_0, red_1);

	/// make the interpolation and set up lookup table
	makeInterpolation_UNORM(red_0, red_1, lookupTable);


/*///////////////////////////////////////////////////
	printf("lookup Color table\n");
	for (int i = 0; i < 8; i++)
		printf("index[%d]:\t%f\n", i, lookupTable[i]);
//////////////////////////////////////////////////*/

	
	/// Calculate the matching values of the color in the lookup table
	calculateClosest(block, lookupTable, indexTable);


/*////////////////////////////////////////////////////////
	printf("\nIndex values\n");
	for (int i = 0; i < 16; i++)
		printf("pixel[%d]: %f - index:\t%d\n", i, block[i] / 255.0f, indexTable[i]);
//////////////////////////////////////////////////////////////*/


	/// Transform the block in  64 bits 
	bc4 = transformTo64Bits(indexTable, red_0, red_1);


/*/////////////////////////////////////////////////////
	printf("\nbc4 64 bits  \n");
	std::cout << "bc4: " << std::bitset<64>(bc4) << "\n";
///////////////////////////////////////////*/

	return bc4;
}


/** Calculate the minimum  and maximum value in block for interpolation
\param	block   4x4 block 
\param	red_0	minimum value	
\param	red_1   maximum value
*/
void BlockCompression::calculateEndpoints(uint8_t* block, uint8_t &p_red_0, uint8_t &p_red_1) {
	
	uint8_t	min = block[0];
	uint8_t max = block[0];

	/// calculate maximun and minimum value in the block
	for (size_t i = 0; i < 16; i++) {
		if (block[i] < min)
			min = block[i];
		else if (block[i] > max)
			max = block[i];
	}

	p_red_0 = max;
	p_red_1 = min;
}

/** Make the interpolation and calculate the lookup table
\param	red_0		minimum value for interpolating
\param	red_1		maximum value for interpolating
\param	lookpTable	reference color table
*/
void BlockCompression::makeInterpolation_UNORM(uint8_t red_0, uint8_t red_1, float* lookupTable) {

	/// Interpolated color values obtained of  
	/// https://msdn.microsoft.com/en-us/library/windows/desktop/bb694531#BC4_UNORM
	/// and create the lookup color table

	float fRed_0	= red_0 / 255.0f;
	float fRed_1	= red_1 / 255.0f;

	lookupTable[0] = fRed_0;
	lookupTable[1] = fRed_1;

	if (red_0 > red_1)
	{
		// 6 interpolated color values
		lookupTable[2] = (6 * fRed_0 + 1 * fRed_1) / 7.0f;  // bit code 010
		lookupTable[3] = (5 * fRed_0 + 2 * fRed_1) / 7.0f;	// bit code 011
		lookupTable[4] = (4 * fRed_0 + 3 * fRed_1) / 7.0f;	// bit code 100
		lookupTable[5] = (3 * fRed_0 + 4 * fRed_1) / 7.0f;	// bit code 101
		lookupTable[6] = (2 * fRed_0 + 5 * fRed_1) / 7.0f;	// bit code 110
		lookupTable[7] = (1 * fRed_0 + 6 * fRed_1) / 7.0f;	// bit code 111
	}
	else
	{
		// 4 interpolated color values
		lookupTable[2] = (4 * fRed_0 + 1 * fRed_1) / 5.0f;	// bit code 010
		lookupTable[3] = (3 * fRed_0 + 2 * fRed_1) / 5.0f;	// bit code 011
		lookupTable[4] = (2 * fRed_0 + 3 * fRed_1) / 5.0f;	// bit code 100
		lookupTable[5] = (1 * fRed_0 + 4 * fRed_1) / 5.0f;	// bit code 101
		lookupTable[6] = 0.0f;								// bit code 110
		lookupTable[7] = 1.0f;								// bit code 111
	}

}


/** Calculate values that most closely matches the original alpha for a given texel.
\param  texel		4x4 block with original data
\param	lookupTable	Color table that contains 8 interpolated colors
\param  blockIndex	Index of color table corrresponding to original data 
*/
void BlockCompression::calculateClosest(uint8_t block[16], float lookupTable[8], uint8_t* blockIndex) {
	
	
	for(size_t texel = 0; texel < 16; texel++){
		float	distance = std::numeric_limits<float>::max();
		uint8_t	bestIndex = 0;
		for (int index = 0; index < 8; index++) {
			float currentDistance = std::fabsf(lookupTable[index] - (block[texel] / 255.0f));
			if (currentDistance < distance) {
				distance = currentDistance;
				bestIndex = index;
			}
		}

		blockIndex[texel] = bestIndex;
	}

}


/** Storing the color values and references in 64 bit
\param  m_indexTable	Index of the reference colors
\param	m_red_0			first word 
\param  m_red_1			second word
*/
uint64_t BlockCompression::transformTo64Bits(uint8_t m_indexTable[16], uint8_t m_red_0, uint8_t m_red_1) {
	
	uint64_t m_bc4 = 0;
	
	//// Insert red p to red a
	for (int index = 15; index >= 0; index--) {
		m_bc4 <<= 3;
		m_bc4 |= (m_indexTable[index] & 7);
	}

	/// Insert red 1 and red 0
	m_bc4 <<= 8;
	m_bc4 |= (m_red_1 & 0xFF);
	m_bc4 <<= 8;
	m_bc4 |= (m_red_0 & 0xFF);
		
	return m_bc4;

}
