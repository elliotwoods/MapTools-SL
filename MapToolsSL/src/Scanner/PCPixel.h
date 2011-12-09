#pragma once
/*
 *  PCPixel.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 14/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCconstants.h"
#include "PCConfig.h"

#include "ofMain.h"
#include <math.h>


class PCPixel
{
	public:
		PCPixel();
		virtual void clear();
	
		virtual void addFind(int iCamPixel, float xCamX, float xCamY);

		//data
		ofPoint xdash;
		int iLastFoundPixel;
		int nFinds;	

};

class PCPixelMeans : public PCPixel, public PCConfig
{
	public:
		void addFind(int iCamPixel, float xCamX, float xCamY);
};

class PCPixelSDev : public PCPixelMeans
{
	public:
		void	clear();

		void	addFind(int iCamPixel, float xCamX, float xCamY);

		void	getFinds(std::vector<ofPoint> **ptrFindsXdash);
		void	getFirstData(float &firstXdash, float &firstYdash) const;

		void	calcDeviation();
		
		ofPoint	sigmaXdash;
		float	sigmaRdash; // standard deviation	

	protected:
		std::vector<int>		_findsIdash;
		std::vector<ofPoint>	_findsXdash;

};