/*
 *  PayloadGraycode.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadGraycode.h"

void PayloadGraycode::setup()
{
	PayloadBase::calcCommon();

	////////////////////////////////////////
	// CALCULATE VALUES
	////////////////////////////////////////
	int nPixelsPerInterleaveX = (projWidth/interleaveWidth);
	int nBitsX = ceil(Log2(projWidth/interleaveWidth));
	int nBitsY = ceil(Log2(projHeight/interleaveHeight));

	dataFramesPerInterleave = nBitsX + nBitsY;
	totalFramesPerInterleave = dataFramesPerInterleave + errorBits;
	totalFrames = totalFramesPerInterleave * interleaveCount;
	maxIndex = pow(2,ceil(Log2(nPixelsPerInterleave)));
	////////////////////////////////////////

	PayloadBase::setup();

	////////////////////////////////////////
	// BUILD DATA ARRAYS
	////////////////////////////////////////
	unsigned long grayCode, grayCodeX, grayCodeY;
	unsigned long iX, iY;
	
	for (unsigned long i=0; i<nPixelsPerInterleave; i++)
	{
		iX = i % nPixelsPerInterleaveX;
		iY = i / nPixelsPerInterleaveX;

		grayCodeX = iX ^ (iX >> 1);
		grayCodeY = iY ^ (iY >> 1);
		grayCode = grayCodeX + (grayCodeY << nBitsX);

		data[i] = grayCode;
		dataInverse[grayCode] =i;

		//make a 32bit random int for the error check
        //mask to error check bits
		errorCheck[i] = data[i] & ((1 << errorBits) - 1);
		
//		//make first 2 frames of error check low/high
//		errorCheck[i] = errorCheck[i] | 1; //high
//		errorCheck[i] = (errorCheck[i] | 2) - 2; //low
		
	}
	////////////////////////////////////////
}

bool PayloadGraycode::decode(unsigned long reading, unsigned long &iX, unsigned long &iY, int subScanWidth, int subScanHeight)
{
	bool valid = (reading > 0) && (reading < maxIndex);

	if (!valid)
		return false;

	unsigned long pixIndex = dataInverse[reading];
				
	iX = pixIndex % subScanWidth;
	iY = pixIndex / subScanWidth;

	valid &= (iX<subScanWidth);
	valid &= (iY<subScanHeight);
	
	return valid;
}