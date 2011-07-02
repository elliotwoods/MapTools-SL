/*
 *  PayloadBase.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PayloadBase.h"

PayloadBase* Payload::Pointer = 0;

PayloadBase::~PayloadBase()
{
    if (isInitialised)
    {
        delete[] data;
        delete[] dataInverse;
        delete[] errorCheck;
    }
}

void PayloadBase::calcCommon()
{
	////////////////////////////////////////
	// CALCULATE COMMON VALUES
	////////////////////////////////////////
	nPixelsPerInterleaveX = projWidth / interleaveWidth;
	nPixelsPerInterleaveY = projHeight / interleaveHeight;
	nPixelsPerInterleave = nPixelsPerInterleaveX * nPixelsPerInterleaveY;

	calibrateFrameCount = 1 + interleaveCount;
	////////////////////////////////////////
}

void PayloadBase::setup()
{
	////////////////////////////////////////
	// CLEAR / SETUP ARRAYS
	////////////////////////////////////////
	if (isInitialised)
	{
		delete[] data;
		delete[] dataInverse;
		delete[] errorCheck;
	}
	data = new unsigned long[maxIndex];
	dataInverse = new unsigned long[maxIndex];
	errorCheck = new unsigned long[maxIndex];
	isInitialised=true;
	////////////////////////////////////////
}


//helper functions
int	PayloadBase::iScanInterleaveFrame(int iFrame)
{
	return iFrame % totalFramesPerInterleave;
}

int PayloadBase::iInterleave(int iFrame)
{
	return iFrame / totalFramesPerInterleave;
}

