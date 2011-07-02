/*
 *  PCPixel.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 14/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCPixel.h"

PCPixel::PCPixel()
{
	clear();
}

void PCPixel::clear()
{
	nFinds=0;
	
	xdash.x=0;
	xdash.y=0;
		
	iLastFoundPixel=-1;
}

void PCPixelSDev::clear()
{
	PCPixel::clear();

	_findsIdash.clear();
	_findsXdash.clear();
}

void PCPixel::addFind(int iPixelDash, float xXdash, float xYdash)
{
	nFinds++;
	xdash.x = xXdash;
	xdash.y = xYdash;
	
	iLastFoundPixel = iPixelDash;
}

void PCPixelMeans::addFind(int iPixelDash, float xXdash, float xYdash)
{
    float xMove = xXdash - xdash.x;
    float yMove = xYdash - xdash.y;
    
    if (xMove * xMove + yMove * yMove > maxPixelMoveSquared)
        return;
    
	nFinds++;
	xdash.x = (xdash.x*(nFinds-1) + xXdash) / nFinds;
	xdash.y = (xdash.y*(nFinds-1) + xYdash) / nFinds;
	
	iLastFoundPixel = iPixelDash;
}

void PCPixelSDev::addFind(int iPixelDash, float xXdash, float xYdash)
{
	PCPixelMeans::addFind(iPixelDash, xXdash, xYdash);

	_findsIdash.push_back(iPixelDash);
	_findsXdash.push_back(ofPoint(xXdash,xYdash));
}

void PCPixelSDev::calcDeviation()
{
	float f_nFinds = nFinds;
	
	sigmaXdash.x=0;
	sigmaXdash.y=0;
	
	for (int iFind = 0; iFind < nFinds; iFind++)
	{
		sigmaXdash.x += (_findsXdash.at(iFind).x - xdash.x) * (_findsXdash.at(iFind).x - xdash.x);
		sigmaXdash.y += (_findsXdash.at(iFind).x - xdash.y) * (_findsXdash.at(iFind).y - xdash.y);
	}
	
	sigmaXdash.x = sqrt(sigmaXdash.x/f_nFinds);
	sigmaXdash.y = sqrt(sigmaXdash.y/f_nFinds);
	
	sigmaRdash = sqrt((sigmaXdash.x*sigmaXdash.x) + (sigmaXdash.y*sigmaXdash.y));
	
}

void PCPixelSDev::getFinds(std::vector<ofPoint> **ptrFindsXdash)
{
	*ptrFindsXdash = &_findsXdash;
}

void PCPixelSDev::getFirstData(float &firstXdash, float &firstYdash) const
{
	if (nFinds>0)
	{
		firstXdash = _findsXdash.at(0).x;
		firstYdash = _findsXdash.at(0).y;
	} else {
		firstXdash = 0;
		firstYdash = 0;
	}
}
