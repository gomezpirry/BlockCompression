/* 
*	Author: Augusto Gomez
*   Description: Block Compression algorithm (BC4)
*/

/**
\file     BlockCompression.h
\brief    compress an b4x4 block with BC4 algorithm
*/


#ifndef _BLOCKCOMPRESSION_
#define _BLOCKCOMPRESSION_

#include <stdint.h>

class BlockCompression
{

private:
	void		calculateEndpoints(uint8_t* block, uint8_t& red_0, uint8_t& red_1);
	void		makeInterpolation_UNORM(uint8_t red_0, uint8_t red_1, float* lookupTable);
	void		calculateClosest(uint8_t block[16], float lookupTable[8], uint8_t* blockIndex);
	uint64_t	transformTo64Bits(uint8_t  indexTable[16], uint8_t m_red_0, uint8_t m_red_1);

public:
	BlockCompression();
	~BlockCompression();

	uint64_t	compressBC4(uint8_t block[16]);
};

#endif // !_BLOCKCOMPRESSION_