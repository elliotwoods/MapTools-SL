/*
 *  CorrelateMain.cpp
 *  MapTools Structured Light
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "CorrelateMain.h"

CorrelateMain::CorrelateMain() :
scrControl("Correlate control"),
scrFileSelection("Select data", ".", "bin"),

scrWorldSpace("World XYZ"),

scrGridMain("Correlate"),
scrGridData("Data pointclouds"),

longImageCount(0)
{
	////////////////////////////
	// Initialise some values
	////////////////////////////
	//
	stereoIntersectThreshold = 0.1;
	//
	////////////////////////////
	
	
	////////////////////////////
	// BUILD INTERFACE
	////////////////////////////
	wdgSlider *wdgLBF = new wdgSlider("Bounds Left, Bottom, Front",
									  scrWorldSpace.lbf,
									  -10, 10,
									  0.05,
									  "m");
    
    wdgSlider *wdgRTB = new wdgSlider("Bounds Right, Top, Back",
									  scrWorldSpace.rtb,
									  -10, 10,
									  0.05,
									  "m");
    
	bangLoad = new wdgButton("Load data");
	bangLoadCalibration = new wdgButton("Load calibration");
	bangEvaluate = new wdgButton("Reproject points");
	
	bangSave3DScan = new wdgButton("Save projection space XYZ");
	bangAddToImage = new wdgButton("Add current projector to Image");
    wdgCounter *counterNProjectors = new wdgCounter("Projectors in Long Image", longImageCount);
	bangSaveImage = new wdgButton("Save image");
    bangClearImage = new wdgButton("Clear image");
	
	bangEvaluate->enabled=false;
	bangSave3DScan->enabled=false;
	
	scrControl.push(bangLoad);
	scrControl.push(bangLoadCalibration);
	scrControl.push(new wdgSlider("Stereo intersect threshold", stereoIntersectThreshold, 0, 0.4, 0.005, "m"));
	scrControl.push(bangEvaluate);
	scrControl.push(new wdgButton("Swap cameras", swapCameras));
    scrControl.push(wdgLBF);
    scrControl.push(wdgRTB);
	scrControl.push(bangSave3DScan);
    scrControl.push(bangAddToImage);
    scrControl.push(counterNProjectors);
    scrControl.push(bangSaveImage);
    scrControl.push(bangClearImage);
	
	
	scrGridData.push(&scrWorldSpace);
	scrGridData.setGridWidth(1);
	
	scrGridMain.push(&scrControl);
	scrGridMain.push(&scrFileSelection);
	scrGridMain.push(&scrGridData);
	scrGridMain.setGridWidth(3);
	////////////////////////////
	
}

void CorrelateMain::update()
{
	if (bangLoad->getBang())
		loadData();
	
	if (bangEvaluate->getBang())
		evaluate();
	
	if (bangLoadCalibration->getBang())
	{
		calibration.load();
		bangEvaluate->enabled = true;
	}
	
	if (bangSave3DScan->getBang())
		save3DScan();
    
    if (bangAddToImage->getBang())
        addToImage();
    
    if (bangSaveImage->getBang())
        saveImage();
    
    if (bangClearImage->getBang())
        clearImage();
	
	ofPoint& lbf(scrWorldSpace.lbf);
    ofPoint& rtb(scrWorldSpace.rtb);
	
	if (rtb.x < lbf.x)
		rtb.x = lbf.x;
	if (rtb.y < lbf.y)
		rtb.y = lbf.y;
	if (rtb.z < lbf.z)
		rtb.z = lbf.z;
	
}

void CorrelateMain::loadData()
{
	nPoints = 0;
	
	char	thisNCameras;
	int		thisFileSize;
	string	thisFilename;
	int		thisNPoints;
	
	int		thisStart, thisEnd;
	float	thisvalx, thisvaly; //for reading from file
	
	for (int iFile=0; iFile<scrFileSelection.nFiles; iFile++)
		if (scrFileSelection.selected[iFile])
		{
			///////////////////////////////
			// Open file
			///////////////////////////////
			//
			thisFilename = ofToDataPath(scrFileSelection.getPath(iFile));
            lastFilename = thisFilename;
			ifstream inFile(thisFilename.c_str(),
							ios::in | ios::binary);
			//
			if (!inFile.is_open())
			{
				ofLog(OF_LOG_WARNING, "CorrelateMain: failed to load file " + thisFilename);
				continue;
			}
			//
			///////////////////////////////
			
			
			///////////////////////////////
			// Determine size
			///////////////////////////////
			//
			thisStart = inFile.tellg();
			inFile.seekg (0, ios::end);
			thisEnd = inFile.tellg();
			//
			thisFileSize = thisEnd - thisStart;
			//
			inFile.seekg(0);
			//
			///////////////////////////////
			
			
			///////////////////////////////
			// Read in number of cameras
			///////////////////////////////
			//
			inFile.read(&thisNCameras, 1);
			inFile.read((char*) &projWidth, 2);
			inFile.read((char*) &projHeight, 2);
			//
			///////////////////////////////
			
			
			///////////////////////////////
			// Read data points
			///////////////////////////////
			//
			thisNPoints = (thisFileSize-1) / (4+8+8*2);
			if (thisNPoints > 0)
			{
				dataCameraSpace.resize(thisNPoints);
				
				//
				for (int iPoint=0; iPoint<thisNPoints; iPoint++)
				{
					//read Indices
					inFile.read((char*) &dataset_iPX[iPoint], 2);
					inFile.read((char*) &dataset_iPY[iPoint], 2);
					
					//read normalised positions (0->1)
					inFile.read((char*) &thisvalx, 4);
					inFile.read((char*) &thisvaly, 4);
					
					for (int iCam=0; iCam<2; iCam++)
					{					
						//read positions
						inFile.read((char*) &thisvalx, 4);
						inFile.read((char*) &thisvaly, 4);
						
						if (!swapCameras)
						{
							dataCameraSpace[iPoint].xy[iCam].x = thisvalx;
							dataCameraSpace[iPoint].xy[iCam].y = thisvaly;
						} else {
							dataCameraSpace[iPoint].xy[1-iCam].x = thisvalx;
							dataCameraSpace[iPoint].xy[1-iCam].y = thisvaly;
						}
						
					}
					
					++nPoints;
				}
			} else {
				ofLogWarning() << "No datapoints in file";
			}
			//
			///////////////////////////////
			
			
			///////////////////////////////
			// Close up the shop
			///////////////////////////////
			//
			inFile.close();
			//
			///////////////////////////////			
			
			//for OpenCV -> we only want to open one here
			break;
		}
	
	
	//clear test set window
	scrWorldSpace.setWith(&dataWorldSpace[0].x, 0);
	
	bangEvaluate->enabled=true;
}


void CorrelateMain::evaluate()
{
    calibration.evaluate(dataCameraSpace, dataWorldSpace, stereoIntersectThreshold);
	scrWorldSpace.setWith(&dataWorldSpace[0].x, dataWorldSpace.size());
	bangSave3DScan->enabled = true;
}

void CorrelateMain::save3DScan()
{
    //////////////////////////
    // BMP file
    //////////////////////////
    //
	ofPixels imgSave;
	imgSave.allocate(projWidth, projHeight, OF_PIXELS_RGB);
	
	//clear all values out to black
	memset(imgSave.getPixels(), 0, projWidth*projHeight*3);
	
    ofPoint& lbf(scrWorldSpace.lbf);
    ofPoint& rtb(scrWorldSpace.rtb);
    
	int iPP, iPoint;
	iPoint = 0;
	unsigned char col[3];
	vector<ofVec3f>::iterator it;
	for (it = dataWorldSpace.begin(); it != dataWorldSpace.end(); ++it)
	{
		const ofVec3f& xyz(*it);
		
        //check if not within selected bounds
        if (xyz[0] < lbf.x || xyz[1] < lbf.y || xyz[2] < lbf.z || xyz[0] > rtb.x || xyz[1] > rtb.y || xyz[2] > rtb.z)
            continue;
		
		//convert position to colour values
		col[0] = ofMap(xyz[0],lbf.x,rtb.x,0,255,true);
		col[1] = ofMap(xyz[1],lbf.y,rtb.y,0,255,true);
		col[2] = ofMap(xyz[2],lbf.z,rtb.z,0,255,true);
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
        
		if (iPP<int(projWidth*projHeight) && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
		
		++iPoint;
	}
	
	//ofSaveImage(imgSave, lastFilename + ".bmp");
    //
    //////////////////////////
	
	
    //////////////////////////
    // HDR file
    //////////////////////////
    //
	ofFloatPixels hdrSave;
	hdrSave.allocate(projWidth, projHeight, OF_PIXELS_RGB);
	imgSave.allocate(projWidth, projHeight, OF_PIXELS_RGB);
	
	//clear all values out to black
	memset(hdrSave.getPixels(), 0, projWidth*projHeight*3 * sizeof(float));
	memset(imgSave.getPixels(), 0, projWidth*projHeight*3 * sizeof(unsigned char));
	
    iPoint = 0;
	for (it = dataWorldSpace.begin(); it != dataWorldSpace.end(); ++it)
	{
		const ofVec3f& xyz(*it);
		
		//convert position to colour values
		col[0] = ofMap(xyz[0],lbf.x,rtb.x,0,255,true);
		col[1] = ofMap(xyz[1],lbf.y,rtb.y,0,255,true);
		col[2] = ofMap(xyz[2],lbf.z,rtb.z,0,255,true);
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
        
		if (iPP<int(projWidth*projHeight) && iPP>=0)
			memcpy(hdrSave.getPixels()+3*iPP, &xyz.x, 3 * sizeof(float));
		
		if (iPP<int(projWidth*projHeight) && iPP>=0)
			memcpy(imgSave.getPixels()+3*iPP, col, 3);
		
		++iPoint;
	}
	
	ofSaveImage(hdrSave, lastFilename + ".hdr");
	ofSaveImage(hdrSave, lastFilename + ".dds");
	ofSaveImage(imgSave, lastFilename + ".png");
    //
    //////////////////////////
    
    
    
    //////////////////////////
    // Binary data
    //////////////////////////
    //
    int nPointsSelected = 0, null;
    
    string filename = lastFilename + ".scan";
    
    ofstream outFile(filename.c_str(), ios::out | ios::binary);
    
    //write overall data
	
    outFile << "3D";
    outFile.write((char*) &null, 4);
    outFile.write((char*) &projWidth, 2);
    outFile.write((char*) &projHeight, 2);
    outFile.write((char*) &scrWorldSpace.lbf.x, 4 * 3);
    outFile.write((char*) &scrWorldSpace.rtb.x, 4 * 3);
	
    iPoint = 0;
	for (it = dataWorldSpace.begin(); it != dataWorldSpace.end(); ++it)
	{
		const ofVec3f& xyz(*it);
        
        //check if not within selected bounds
        if (xyz[0] < lbf.x || xyz[1] < lbf.y || xyz[2] < lbf.z || xyz[0] > rtb.x || xyz[1] > rtb.y || xyz[2] > rtb.z)
            continue;
        
        outFile.write((char*) &dataset_iPX[iPoint], 4);
        outFile.write((char*) &dataset_iPY[iPoint], 4);
        outFile.write((char*) &xyz, 4 * 3);
        
        nPointsSelected++;
		++iPoint;
    }
    
    outFile.seekp(2);
    outFile.write((char*) &nPointsSelected, 4);    
    outFile.close();
    //
    //////////////////////////
}

void CorrelateMain::addToImage()
{
    int oldLongImageCount = longImageCount;
    int oldWidth = projWidth * oldLongImageCount;
    longImageCount++;
    int newWidth = projWidth * longImageCount;    
    
    //setup new image
	ofFloatPixels newImage;
	newImage.allocate(projWidth*longImageCount,projHeight, OF_PIXELS_RGB);
    memset(newImage.getPixels(), 0, 3*projWidth*projHeight*longImageCount*sizeof(float));
    
    //copy old contents around
    for (int j = 0; j < projHeight; j++)
        memcpy(newImage.getPixels() + newWidth*3*j*sizeof(float), longImage.getPixels() + oldWidth*3*j*sizeof(float), oldWidth * 3 *sizeof(float));
    
    
    
    //////////////////////////
    // copy new contents in
    //////////////////////////
    //	
    ofPoint& lbf(scrWorldSpace.lbf);
    ofPoint& rtb(scrWorldSpace.rtb);
    
	int iPP, iLIP;
    float* point;
	
	vector<ofVec3f>::iterator it;
	int iPoint = 0;
	for (it = dataWorldSpace.begin(); it != dataWorldSpace.end(); ++it)
	{
		const ofVec3f& xyz(*it);
        
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
        
		if (iPP < int(projWidth*projHeight) && iPP>=0)
        {
            //put pixels at right of image
            iLIP =  dataset_iPX[iPoint] + newWidth * dataset_iPY[iPoint] + oldWidth;
            
			memcpy(newImage.getPixels() + 3*iLIP, &xyz.x, 3*sizeof(float));
        }
		
		++iPoint;
	}
    //
    //////////////////////////
    
    longImage = newImage;
    
    
}

void CorrelateMain::saveImage()
{
	/*
	 ofBuffer buf;
	 ofSaveImage(longImage, buf, OF_IMAGE_FORMAT_HDR, OF_IMAGE_QUALITY_BEST);
	 ofBufferToFile("long image.hdr", buf, true);
	 */
	ofSaveImage(longImage, "longimage.hdr", OF_IMAGE_QUALITY_BEST);
	
}

void CorrelateMain::clearImage()
{
    if (longImageCount > 0)
    {
        longImage.clear();
        longImageCount = 0;
    }
	
}