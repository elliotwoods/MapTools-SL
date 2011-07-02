//
//  ScanSet.cpp
//  PC Encode
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ScanSet.h"

ScanSet::ScanSet() :
isAllocated(false),
nPoints(0),
hasBounds(false)
{
    
}

ScanSet::~ScanSet()
{
    deAllocate();
}

void ScanSet::allocate(int count)
{
    nPoints = count;
    
    allocate();
}

void ScanSet::allocate()
{
    if (isAllocated)
        deAllocate();

    xyz = new float[3*nPoints];
    rgb = new float[3*nPoints];

    iX = new unsigned int[nPoints];
    iY = new unsigned int[nPoints];

    isAllocated = true;
}

void ScanSet::deAllocate()
{
    if (!isAllocated)
        return;
    
    delete[] xyz;
    delete[] rgb;
    delete[] iX;
    delete[] iY;
    
    isAllocated = false;
}

void ScanSet::setup(ScanSet &other)
{
    width = other.width;
    height = other.height;
    
    memcpy(lbf, other.lbf, 4*3);
    memcpy(rtb, other.rtb, 4*3);
}


void ScanSet::calcRGB()
{
    if (!isAllocated)
        return;
    
    for (int i=0; i<nPoints; i++)
    {
        rgb[i*3 + 0] = (xyz[i*3 + 0] - lbf[0]) / (rtb[0] - lbf[0]);
        rgb[i*3 + 1] = (xyz[i*3 + 1] - lbf[1]) / (rtb[1] - lbf[1]);
        rgb[i*3 + 2] = (xyz[i*3 + 2] - lbf[2]) / (rtb[2] - lbf[2]);
    }    
}

void ScanSet::loadBin(string filename)
{
    //////////////////////////
    // Load binary data
    //////////////////////////
    //
    ifstream inFile(filename.c_str(), ios::out | ios::binary);
    
    char header[2];
    
    //read overall data
    inFile.read(header, 2);
    
    if (header[0] != '3' || header[1] != 'D')
    {
        ofLog(OF_LOG_ERROR, "ScanSet: Failed to load scan binary file, incorrect header");
        return;
    }
    
    inFile.read((char*) &nPoints, 4);
    inFile.read((char*) &width, 2);
    inFile.read((char*) &height, 2);
    inFile.read((char*) lbf, 4 * 3);
    inFile.read((char*) rtb, 4 * 3);
    
    allocate(nPoints);    
    
    for (int i=0; i<nPoints; i++)
    {
        inFile.read((char*) &iX[i], 4);
        inFile.read((char*) &iY[i], 4);
        inFile.read((char*) &xyz[i*3], 4 * 3);
    }
    
    inFile.close();
    //
    //////////////////////////
    
    loadFilename = filename;
    
    calcRGB();
}

void ScanSet::saveBin(string filename)
{
    //////////////////////////
    // Binary data
    //////////////////////////
    //
    int nPointsSelected = 0, null;
    
    ofstream outFile(filename.c_str(), ios::out | ios::binary);
    
    //write overall data
    
    outFile << "3D";
    outFile.write((char*) &null, 4);
    outFile.write((char*) &width, 2);
    outFile.write((char*) &height, 2);
    outFile.write((char*) lbf, 4 * 3);
    outFile.write((char*) rtb, 4 * 3);
    
    float* point;
    
    for (int i=0; i<nPoints; i++)
    {
        point = &xyz[i*3];
        
        //check if not within selected bounds
        if (hasBounds)
            if (point[0] < lbf[0] || point[1] < lbf[1] || point[2] < lbf[2] || point[0] > rtb[0] || point[1] > rtb[1] || point[2] > rtb[2])
                continue;
        
        outFile.write((char*) &iX[i], 4);
        outFile.write((char*) &iY[i], 4);
        outFile.write((char*) point, 4 * 3);
        
        nPointsSelected++;
    }
    
    outFile.seekp(2);
    outFile.write((char*) &nPointsSelected, 4);    
    outFile.close();
    //
    //////////////////////////  
}
void ScanSet::saveImage(string filename)
{
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
    float* point;
	
	for (int iPoint=0; iPoint<nPoints; iPoint++)
	{
        point = &xyz[iPoint*3];
        
        //check if not within selected bounds
        if (hasBounds)
            if (point[0] < lbf[0] || point[1] < lbf[1] || point[2] < lbf[2] || point[0] > rtb[0] || point[1] > rtb[1] || point[2] > rtb[2])
                continue;
        
		//convert position to colour values
		col[0] = ofMap(point[0],lbf[0],rtb[0],0,255,true);
		col[1] = ofMap(point[1],lbf[1],rtb[1],0,255,true);
		col[2] = ofMap(point[2],lbf[2],rtb[2],0,255,true);
		
		iPP = iX[iPoint] + width * iY[iPoint];
        
		if (iPP<int(width * height) && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
	}
	
	imgSave.saveImage(filename);
    //
    //////////////////////////
}

void ScanSet::operator=(ScanSet& other)
{
    setup(other);
    nPoints = other.nPoints;
    loadFilename = other.loadFilename;
    
    allocate();
    
    memcpy(xyz, other.xyz, nPoints * 4 * 3);
    memcpy(rgb, other.rgb, nPoints * 4 * 3);
    
    memcpy(iX, other.iX, nPoints * 4);
    memcpy(iY, other.iY, nPoints * 4);
}