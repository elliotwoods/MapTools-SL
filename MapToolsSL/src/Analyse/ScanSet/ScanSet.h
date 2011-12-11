#pragma once
//
//  ScanSet.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofMain.h"

#include "TalkyBuffer.h"

#define SCANSET_EXTENSION string("points")
#define SCANSET_LOCATION string("Logs")

using namespace Talky;

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


	void    initialise(const ScanSet &other, int size);	
	void    initialise(int width, int height, int size);

	///for use when you set nPoints manually
	void    allocate();
	void    deAllocate();

	///call this is you update the contents of the set
	void	update();

	void    load(string filename);
	void    save(string filename, bool thresholdBounds=true);

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
	unsigned int	size;
	unsigned int	active;

	unsigned short  width, height;

	bool            hasBounds;
	ofVec3f			lbf, rtb;

	bool            isAllocated;
	//
	////
	
	string          loadFilename;

	ofEvent<unsigned int>	onUpdate;

private:
	bool			inside(ofVec3f& point, ofVec3f &lbf, ofVec3f& rtb);
    
};