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
size(0),
active(0),
hasBounds(false) {
    
}

ScanSet::~ScanSet() {
    deAllocate();
}

void ScanSet::initialise(int width, int height, int size) {
    this->size = size;
	this->width = width;
	this->height = height;
    
    allocate();
}

void ScanSet::allocate() {
    if (isAllocated)
        deAllocate();

    xyz = new ofVec3f[size];
    iX = new unsigned int[size];
    iY = new unsigned int[size];

    isAllocated = true;

	update();
}

void ScanSet::deAllocate() {
    if (!isAllocated)
        return;
    
    delete[] xyz;
    delete[] iX;
    delete[] iY;
    
	size = 0;

    isAllocated = false;

	update();
}

void ScanSet::update() {
	ofNotifyEvent(onUpdate, size, this);
}

void ScanSet::initialise(const ScanSet &other, int size) {
    this->width = other.width;
    this->height = other.height;
    this->size = size;

	lbf = other.lbf;
	rtb = other.rtb;
	allocate();
}

void ScanSet::load(string filename) {
	
	fstream file;
	file.open(filename, fstream::in | fstream::binary);

	if (!file.is_open()) {
		ofLogError() << "ScanSet::loadBin : loading file " << filename << " failed. Check that it's not locked or missing.";
		return;
	}
	char head[2];

	file.read(head, 2);
	
    if (!(head[0] == '3' && head[1] == 'D'))
    {
        ofLog(OF_LOG_ERROR, "ScanSet: Failed to load scan binary file, incorrect header");
        return;
    }
	
	file.read((char*)&size, 4);
	file.read((char*)&width, 4);
	file.read((char*)&height, 4);
	file.read((char*)&lbf, 12);
	file.read((char*)&rtb, 12);
	
    this->allocate();    
    
    for (int i=0; i<size; i++) {
		file.read((char*)&iX[i], 4);
		file.read((char*)&iX[i], 4);
		file.read((char*)&xyz[i], 12);
    }

	file.close();
    
    loadFilename = filename;
	update();
}

bool ScanSet::inside(ofVec3f& point, ofVec3f &lbf, ofVec3f& rtb) {
	return !(point.x < lbf.x || point.y < lbf[1] || point.z < lbf[2] || point.x > rtb.x || point.y > rtb[1] || point.z > rtb[2]);
}

void ScanSet::save(string filename, bool thresholdBounds) {
	
	filename = ofFile(filename).getBaseName() + "." + SCANSET_EXTENSION;

	filename = SCANSET_LOCATION + 
				#ifdef TARGET_WIN32
					"\\"
				#else
					"/"
				#endif
					+ filename;
	filename = ofToDataPath(filename, true);

	fstream file(filename, fstream::out | fstream::binary | fstream::trunc);

    //////////////////////////
    // Binary data
    //////////////////////////
    //
    int nPointsSelected = 0;
	int blank = 0;
	char header[2] = {'3', 'D'};
    //write overall data
    
	file.write(header, 2);
    
	file.write((char*)&blank, 4); //we come back write this later (number of points selected)
	file.write((char*)&width, 4);
	file.write((char*)&height, 4);
	file.write((char*)&lbf, 12);
	file.write((char*)&rtb, 12);
    
    for (int i=0; i<size; i++)
    {
        //check if not within selected bounds
        if (hasBounds && thresholdBounds)
            if (!inside(xyz[i],lbf,rtb))
                continue;
        file.write((char*)&iX[i], 4);
		file.write((char*)&iY[i], 4);
		file.write((char*)&xyz[i], 12);

		nPointsSelected++;
    }
    
	file.seekp(2);
	file.write((char*)&nPointsSelected, 4);
	file.close();
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
	
	for (int iPoint=0; iPoint<size; iPoint++)
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
    initialise(other, other.size);
    loadFilename = other.loadFilename;
    
    allocate();
    
    memcpy(xyz, other.xyz, size * sizeof(ofVec3f));
    memcpy(iX, other.iX, size * 4);
    memcpy(iY, other.iY, size * 4);
}

ofVec3f& ScanSet::operator[](const unsigned int i) {
	return xyz[i];
}

void ScanSet::customDraw() {
	ofDrawAxis(100);
}