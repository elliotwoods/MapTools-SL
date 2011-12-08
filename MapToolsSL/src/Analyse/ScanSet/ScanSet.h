#pragma once
//
//  ScanSet.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofMain.h"

#include "ofxTalky.h"

class ScanSetHeader {
public:
	ScanSetHeader();
	bool operator==(const ScanSetHeader &other);
	
	char header[2];
};

class ScanSet : public ofNode
{
public:
	ScanSet();
	~ScanSet();

	void    allocate();
	void    allocate(int count);
	void    deAllocate();

	void    setup(ScanSet &other);

	void    loadBin(string filename);
	void    saveBin(string filename, bool thresholdBounds=true);

	void    saveImage(string filename, bool thresholdBounds=true);

	void		operator=(ScanSet& other);
	ofVec3f&	operator[](const unsigned int i);

	void	customDraw();

	
	////
	//dataset
	//
	ScanSetHeader	header;
	ofVec3f			*xyz;
	unsigned int	*iX, *iY;
	unsigned int	nPoints;

	unsigned short  width, height;

	bool            hasBounds;
	ofVec3f			lbf, rtb;

	bool            isAllocated;
	//
	////
	
	string          loadFilename;

private:
	bool			inside(ofVec3f& point, ofVec3f &lbf, ofVec3f& rtb);
    
};