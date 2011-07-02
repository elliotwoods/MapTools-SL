/*
 *  PayloadBinary.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBinary.h"

void PayloadBinary::setup()
{
	PayloadBase::calcCommon();

	////////////////////////////////////////
	// CALCULATE VALUES
	////////////////////////////////////////
	dataFramesPerInterleave = ceil(Log2(nPixelsPerInterleave));
	totalFramesPerInterleave = dataFramesPerInterleave + errorBits;
	totalFrames = totalFramesPerInterleave * interleaveCount;
	maxIndex = nPixelsPerInterleave;
	////////////////////////////////////////

	PayloadBase::setup();

	////////////////////////////////////////
	// BUILD DATA ARRAYS
	////////////////////////////////////////
	for (int i=0; i<nPixelsPerInterleave; i++)
	{
		//for binary, we just output the binary representation
		//of the pixel's index
		data[i]=i;
		dataInverse[i]=i;

		//make a 64bit random int for the error check
		errorCheck[i] = rand() + (rand()*(long long int)(1)<<32);
		
		
		//make first 2 frames of error check low/high
		errorCheck[i] = errorCheck[i] | 1; //high
		errorCheck[i] = (errorCheck[i] | 2) - 2; //low
	}
	////////////////////////////////////////
}

bool PayloadBinary::decode(unsigned long reading, unsigned long &iX, unsigned long &iY, int subScanWidth, int subScanHeight)
{
	bool valid = (reading > 0) && (reading < maxIndex);
				
	iX = reading % subScanWidth;
	iY = reading / subScanWidth;
	
	return valid;
}