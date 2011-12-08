//
//  ScanSet.cpp
//  MapTools Structured Light
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ScanSet.h"

ScanSetHeader::ScanSetHeader()  {
	header[0] = '3';
	header[1] = 'D';
}

bool ScanSetHeader::operator==(const ScanSetHeader &other) {
	return this->header[0] == other.header[0] && this->header[1] == other.header[1];
}

//--------------------------------------

ScanSet::ScanSet() :
isAllocated(false),
nPoints(0),
hasBounds(false) {
    
}

ScanSet::~ScanSet() {
    deAllocate();
}

void ScanSet::allocate(int count) {
    nPoints = count;
    
    allocate();
}

void ScanSet::allocate() {
    if (isAllocated)
        deAllocate();

    xyz = new ofVec3f[nPoints];
    iX = new unsigned int[nPoints];
    iY = new unsigned int[nPoints];

    isAllocated = true;
}

void ScanSet::deAllocate() {
    if (!isAllocated)
        return;
    
    delete[] xyz;
    delete[] iX;
    delete[] iY;
    
    isAllocated = false;
}

void ScanSet::setup(ScanSet &other) {
    width = other.width;
    height = other.height;
    
	lbf = other.lbf;
	rtb = other.rtb;
}

void ScanSet::loadBin(string filename) {
	
	TalkyBuffer buffer;
	if (!buffer.loadFile(ofToDataPath(filename))) {
		ofLogError() << "ScanSet::loadBin : loading file " << filename << " failed. Check that it's not locked or missing.";
		return;
	}
	
	buffer >> header;
	
    if (!(header == ScanSetHeader()))
    {
        ofLog(OF_LOG_ERROR, "ScanSet: Failed to load scan binary file, incorrect header");
        return;
    }
	
	buffer >> nPoints;
	buffer >> width;
	buffer >> height;
	buffer >> lbf;
	buffer >> rtb;
	
    allocate(nPoints);    
    
    for (int i=0; i<nPoints; i++) {
		buffer >> iX[i];
		buffer >> iX[i];
		buffer >> xyz[i];
    }
    
    loadFilename = filename;
}

bool ScanSet::inside(ofVec3f& point, ofVec3f &lbf, ofVec3f& rtb) {
	return !(point.x < lbf.x || point.y < lbf[1] || point.z < lbf[2] || point.x > rtb.x || point.y > rtb[1] || point.z > rtb[2]);
}

void ScanSet::saveBin(string filename, bool thresholdBounds) {
	
    //////////////////////////
    // Binary data
    //////////////////////////
    //
    int nPointsSelected = 0, null;
    
    ofstream outFile(filename.c_str(), ios::out | ios::binary);
    
    //write overall data
    
    outFile << "3D";
	outFile << null; //we come back write this later
	outFile << width;
	outFile << height;
	outFile << lbf;
	outFile << rtb;
    
    for (int i=0; i<nPoints; i++)
    {
        
        //check if not within selected bounds
        if (hasBounds && thresholdBounds)
            if (!inside(xyz[i],lbf,rtb))
                continue;
        
        outFile.write((char*) &iX[i], 4);
        outFile.write((char*) &iY[i], 4);
        outFile.write((char*) &xyz[i], sizeof(ofVec3f));
        
        nPointsSelected++;
    }
    
    outFile.seekp(2);
    outFile.write((char*) &nPointsSelected, 4);    
    outFile.close();
    //
    //////////////////////////  
}

void ScanSet::saveImage(string filename, bool thresholdBounds) {
    //////////////////////////
    // BMP file
    //////////////////////////
    //
	ofImage imgSave;
	imgSave.setImageType(OF_IMAGE_COLOR);
	imgSave.allocate(width, height, OF_IMAGE_COLOR);
	
	//clear all values out to black
	memset(imgSave.getPixels(), 0, width * height * 3);
    
	int iPP;
	unsigned char col[3];
    ofVec3f* point;
	
	for (int iPoint=0; iPoint<nPoints; iPoint++)
	{
        point = &xyz[iPoint];
        
        //check if not within selected bounds
        if (hasBounds && thresholdBounds)
			if (!inside(*point, lbf, rtb))
                continue;
        
		//convert position to colour values
		col[0] = ofMap(point->x,lbf.x,rtb.x,0,255,true);
		col[1] = ofMap(point->y,lbf.y,rtb.y,0,255,true);
		col[2] = ofMap(point->z,lbf.z,rtb.z,0,255,true);
		
		iPP = iX[iPoint] + width * iY[iPoint];
        
		if (iPP<int(width * height) && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
	}
	
	imgSave.saveImage(filename);
    //
    //////////////////////////
}

void ScanSet::operator=(ScanSet& other) {
    setup(other);
    nPoints = other.nPoints;
    loadFilename = other.loadFilename;
    
    allocate();
    
    memcpy(xyz, other.xyz, nPoints * sizeof(ofVec3f));
    
    memcpy(iX, other.iX, nPoints * 4);
    memcpy(iY, other.iY, nPoints * 4);
}

ofVec3f& ScanSet::operator[](const unsigned int i) {
	return xyz[i];
}

void ScanSet::customDraw() {
	ofDrawAxis(100);
}