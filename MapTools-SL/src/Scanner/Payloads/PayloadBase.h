#pragma once
/*
 *  PayloadBase.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 15/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include <cmath>
#include "PCConfig.h"

class PayloadBase : public PCConfig
{
public:

	////////////////////////////////////////
	// VALUES
	////////////////////////////////////////
	int			nPixelsPerInterleave;
	int			nPixelsPerInterleaveX, nPixelsPerInterleaveY;
	
	int			calibrateFrameCount;
	int			totalFrames;
	int			totalFramesPerInterleave;
	int			dataFramesPerInterleave;
	
	unsigned long			*data;
	unsigned long			*dataInverse;
	unsigned long			*errorCheck;

	int			maxIndex;
	////////////////////////////////////////

	////////////////////////////////////////
	// FUCNTIONS
	////////////////////////////////////////
	PayloadBase()		{isInitialised=false;};
    ~PayloadBase();
    
	void				calcCommon();
	virtual void		setup();
	virtual bool		decode(unsigned long reading, unsigned long &iX, unsigned long &iY, int subScanWidth, int subScanHeight) = 0;
	
	int					iScanInterleaveFrame(int iFrame);
	int					iInterleave(int iFrame);
	////////////////////////////////////////

protected:
	bool		isInitialised;
	void		clear();
	double		Log2(double n) { return log( n ) / log((double)2); };
};

class Payload
{
    public:
        static PayloadBase* Pointer;
};