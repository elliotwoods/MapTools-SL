/*
 *  CorrelateMain.cpp
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "CorrelateMain.h"

CorrelateMain::CorrelateMain() :
scrControl("Calibrate control"),
scrFileSelection("Select data", "./Logs/", "bin"),

scrInputPoints("Input data"),
scrTestCorrelate("Test correlation"),

scrGridMain("Correlate"),
scrGridData("Data pointclouds"),

longImageCount(0)
{
	////////////////////////////
	// Initialise some values
	////////////////////////////
	//
	nCameras = 0;
	// from the samsung monitor in the studio
	screenWidth = 0.598;
	screenHeight = 0.336;
	//
	polyOrder = 4;
	nDatasets = 0;
	//
	newFormat = true;
	//
	////////////////////////////

	
	////////////////////////////
	// BUILD INTERFACE
	////////////////////////////
	wdgCounter *wdgCameraCount = new wdgCounter("Number of cameras",
										nCameras,
										  0);
	wdgCounter *wdgDatasetCount = new wdgCounter("Datasets loaded",
												 nDatasets,
												 0);
	wdgSlider *wdgScreenWidth = new wdgSlider("Screen width",
											   screenWidth,
											   0, 5,
											   0.001,
											   "meters");
	wdgSlider *wdgScreenHeight = new wdgSlider("Screen height",
											  screenHeight,
											  0, 4,
											  0.001,
											  "meters");
	wdgSlider *wdgPolyOrder = new wdgSlider("Polynomial order",
											polyOrder,
											1, 10,
											1);
    
	wdgSlider *wdgLBF = new wdgSlider("Bounds Left, Bottom, Front",
                                scrTestCorrelate.lbf,
                                -10, 10,
                                0.05,
                                "m");
    
    wdgSlider *wdgRTB = new wdgSlider("Bounds Right, Top, Back",
                                scrTestCorrelate.rtb,
                                -10, 10,
                                0.05,
                                "m");
    
	bangLoad = new wdgButton("Load data");
	bangCorrelate = new wdgButton("Run polyfit");
	bangEvaluate = new wdgButton("Evaluate dataset");
	bangLoadFit = new wdgButton("Load fit");
	bangSaveFit = new wdgButton("Save fit");
	bangSave3DScan = new wdgButton("Save projection space XYZ");
	bangAddToImage = new wdgButton("Add current projector to Image");
    wdgCounter *counterNProjectors = new wdgCounter("Projectors in Long Image", longImageCount);
	bangSaveImage = new wdgButton("Save image");
    bangClearImage = new wdgButton("Clear image");
	
	bangCorrelate->enabled=false;
	bangEvaluate->enabled=false;
	bangSaveFit->enabled=false;
	bangSave3DScan->enabled=false;
	
	scrControl.push(wdgCameraCount);
	scrControl.push(wdgDatasetCount);
	scrControl.push(bangLoad);
	scrControl.push(bangCorrelate);
	scrControl.push(wdgScreenWidth);
	scrControl.push(wdgScreenHeight);
	scrControl.push(wdgPolyOrder);
	scrControl.push(bangEvaluate);
	scrControl.push(bangLoadFit);
	scrControl.push(bangSaveFit);
	scrControl.push(new wdgButton("File format v0.2", newFormat));
	scrControl.push(new wdgButton("Swap cameras", swapCameras));
    scrControl.push(wdgLBF);
    scrControl.push(wdgRTB);
	scrControl.push(bangSave3DScan);
    scrControl.push(bangAddToImage);
    scrControl.push(counterNProjectors);
    scrControl.push(bangSaveImage);
    scrControl.push(bangClearImage);
        
	
	
	scrGridData.push(&scrInputPoints);
	scrGridData.push(&scrTestCorrelate);
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
	
	if (bangCorrelate->getBang())
		runPolyfit();

	if (bangEvaluate->getBang())
		evaluate();
	
	if (bangLoadFit->getBang())
	{
		string fname = "calib.fit";
		fit.load(fname);
		bangEvaluate->enabled = true;
	}

	if (bangSaveFit->getBang())
	{
		//string fname =	"order=" + ofToString(polyOrder, 0) +
		//				",nSets=" + ofToString(nDatasets, 0);
		string fname = "calib.fit";
		fit.save(fname);
	}
	
	if (bangSave3DScan->getBang())
		save3DScan();
    
    if (bangAddToImage->getBang())
        addToImage();
    
    if (bangSaveImage->getBang())
        saveImage();
    
    if (bangClearImage->getBang())
        clearImage();
}

void CorrelateMain::loadData()
{
	dataSet.clear();

	pfitDataPoint<DataType> dataRow = pfitDataPoint<DataType>(1,1);
	nDatasets = 0;
	nPoints = 0;
	
	char	thisNCameras;
	int		thisFileSize;
	string	thisFilename;
	int		thisNPoints;
	float	thisDepth;
	
	int		thisStart, thisEnd;
	float	thisvalx, thisvaly; //for reading from file
	
	for (int iFile=0; iFile<scrFileSelection.nFiles; iFile++)
		if (scrFileSelection.selected[iFile])
		{
			///////////////////////////////
			// Open file
			///////////////////////////////
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
			///////////////////////////////
			
			
			///////////////////////////////
			// Determine size
			///////////////////////////////
			thisStart = inFile.tellg();
			inFile.seekg (0, ios::end);
			thisEnd = inFile.tellg();
			//
			thisFileSize = thisEnd - thisStart;
			//
			inFile.seekg(0);
			///////////////////////////////
			
			
			///////////////////////////////
			// Read in number of cameras
			///////////////////////////////
			inFile.read(&thisNCameras, 1);
			if (newFormat)
			{
				inFile.read((char*) &projWidth, 2);
				inFile.read((char*) &projHeight, 2);
			}
			if (nDatasets==0)
			{
				//first file, so initalise rows
				nCameras = thisNCameras;
				dataSet.init(2*nCameras, 3, 0);
			} else
				if (nCameras != thisNCameras)
					ofLog(OF_LOG_ERROR, "CorrelateMain: mismatched number of cameras between files");
			///////////////////////////////
			
			
			///////////////////////////////
			// Read data points
			///////////////////////////////
			thisNPoints = (thisFileSize-1) / ((newFormat ? 4 : 0)+8+8*nCameras);
			thisDepth = getDepthFromFilename(scrFileSelection.getName(iFile));
			if (thisNPoints > 0)
			{
				//
				pfitIndex oldSize = dataSet.size();
				pfitIndex newSize = dataSet.size() + thisNPoints;
				dataSet.resize(newSize);
				dataRow = dataSet[oldSize];
				//
				for (int iPoint=0; iPoint<thisNPoints; iPoint++)
				{
					if (newFormat)
					{
						//read Indices
						inFile.read((char*) &dataset_iPX[iPoint], 2);
						inFile.read((char*) &dataset_iPY[iPoint], 2);
					}

					//read positions
					inFile.read((char*) &thisvalx, 4);
					inFile.read((char*) &thisvaly, 4);
				
					dataRow.getOutput()[0] = screenWidth * (thisvalx - 0.5);
					dataRow.getOutput()[1] = screenHeight * (thisvaly - 0.5);
					dataRow.getOutput()[2] = thisDepth;
				
					for (int iCam=0; iCam<nCameras; iCam++)
					{					
						//read positions
						inFile.read((char*) &thisvalx, 4);
						inFile.read((char*) &thisvaly, 4);
					
						if (!swapCameras)
						{
							dataRow.getInput()[0 + iCam*2] = thisvalx;
							dataRow.getInput()[1 + iCam*2] = thisvaly;
						} else {
							dataRow.getInput()[0 + (nCameras-iCam-1)*2] = thisvalx;
							dataRow.getInput()[1 + (nCameras-iCam-1)*2] = thisvaly;
						}

					}
				
					++nPoints;
					++dataRow;
				}
				///////////////////////////////
			} else {
				ofLogWarning() << "No datapoints in file at depth " << thisDepth;
			}
			
			///////////////////////////////
			// Close up the shop
			///////////////////////////////
			inFile.close();
			nDatasets++;
			///////////////////////////////			
		}
	
	//load input window
	copyToInputScreen();
	
	//clear test set window
	//**HACK**scrTestCorrelate.setWith(dataSet.getOutput(), dataSet.getOutput(), 0);
	
	bangCorrelate->enabled=true;
	bangSaveFit->enabled=false;
}

float CorrelateMain::getDepthFromFilename(string filename)
{
	return ofToFloat(filename.substr(0,filename.length()-4));
}

void CorrelateMain::copyToInputScreen()
{
	
	if (nPoints>MAXPOINTS)
		ofLog(OF_LOG_WARNING, "CorrelateMain: nPoints > MAXPOINTS. only drawing first MAXPOINTS");

	//**HACK**if (dataSet.size() > 0)
		//**HACK**scrInputPoints.setWith(dataSet.begin().getOutput(), dataSet.begin().getOutput(), MIN(dataSet.size(), MAXPOINTS));	

}

void CorrelateMain::runPolyfit()
{
	fit.init(polyOrder, 2*nCameras, 3, BASIS_SHAPE_TRIANGLE);
	fit.correlate(dataSet);
	ofLogNotice() << "fit residual = " << fit.residualRMS(dataSet);
	
	bangEvaluate->enabled=true;
	bangSaveFit->enabled=true;
}

void CorrelateMain::evaluate()
{
    //HARDCODED FOR 2 CAMERAS
    
	evaluateSet.init(4, 3, dataSet.size());
	memcpy(evaluateSet.getInput(), dataSet.getInput(), dataSet.size() * 4 * sizeof(DataType));
	fit.evaluate(evaluateSet);
	
	//**HACK**scrTestCorrelate.setWith(evaluateSet.getOutput(), evaluateSet.getOutput(), nPoints);
	
	//we'll enable the save xyz, if we're in new format
	bangSave3DScan->enabled = newFormat;
}

void CorrelateMain::save3DScan()
{
    //////////////////////////
    // BMP file
    //////////////////////////
    //
	ofImage imgSave;
	imgSave.setImageType(OF_IMAGE_COLOR);
	imgSave.allocate(projWidth, projHeight, OF_IMAGE_COLOR);
	
	//clear all values out to black
	memset(imgSave.getPixels(), 0, projWidth*projHeight*3);
	
    ofPoint& lbf(scrTestCorrelate.lbf);
    ofPoint& rtb(scrTestCorrelate.rtb);
    
	int iPP, iPoint;
	unsigned char col[3];
    pfitDataPoint<DataType> dataPoint;
	DataType* xyz;
	
	for (dataPoint = dataSet.begin(); dataPoint != dataSet.end(); ++dataPoint)
	{
		xyz = dataPoint.getOutput();
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
	
	imgSave.saveImage(lastFilename + ".bmp");
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
    outFile.write((char*) &scrTestCorrelate.lbf.x, 4 * 3);
    outFile.write((char*) &scrTestCorrelate.rtb.x, 4 * 3);

    iPoint = 0;
	for (dataPoint = dataSet.begin(); dataPoint != dataSet.end(); ++dataPoint)
	{
		xyz = dataPoint.getOutput();
        
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
    unsigned char * newImage = new unsigned char[projWidth*projHeight*3*longImageCount];
    memset(newImage, 0, 3*projWidth*projHeight*longImageCount);
    
    
    
    //copy old contents around
    for (int j = 0; j < projHeight; j++)
        memcpy(newImage + newWidth*3*j, longImage + oldWidth*3*j, oldWidth * 3);
    
    
    
    //////////////////////////
    // copy new contents in
    //////////////////////////
    //	
    ofPoint& lbf(scrTestCorrelate.lbf);
    ofPoint& rtb(scrTestCorrelate.rtb);
    
	int iPP, iLIP;
	unsigned char col[3];
    float* point;
	
	DataType* xyz;
	pfitDataPoint<DataType> dataPoint;
	int iPoint = 0;

	for (dataPoint = dataSet.begin(); dataPoint != dataSet.end(); ++dataPoint)
	{
		xyz = dataPoint.getOutput();
        
        //check if not within selected bounds
        if (point[0] < lbf.x || point[1] < lbf.y || point[2] < lbf.z || point[0] > rtb.x || point[1] > rtb.y || point[2] > rtb.z)
            continue;
        
		//convert position to colour values
		col[0] = ofMap(xyz[0],lbf.x,rtb.x,0,255,true);
		col[1] = ofMap(xyz[1],lbf.y,rtb.y,0,255,true);
		col[2] = ofMap(xyz[2],lbf.z,rtb.z,0,255,true);
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
        
		if (iPP < int(projWidth*projHeight) && iPP>=0)
        {
            //put pixels at right of image
            iLIP =  dataset_iPX[iPoint] + newWidth * dataset_iPY[iPoint] + oldWidth;
            
			memcpy(newImage + 3*iLIP, col, 3);
        }

		++iPoint;
	}
    //
    //////////////////////////
    
    
    //delete old image and reassign
    if (longImageCount > 0)
        delete[] longImage;
    longImage = newImage;
    
    
}

void CorrelateMain::saveImage()
{
    ofImage imgSave;
	imgSave.setImageType(OF_IMAGE_COLOR);
	imgSave.allocate(projWidth * longImageCount, projHeight, OF_IMAGE_COLOR);
    
    memcpy(imgSave.getPixels(), longImage, projWidth*projHeight*longImageCount*3);
    
    imgSave.saveImage("long image.bmp");
}

void CorrelateMain::clearImage()
{
    if (longImageCount > 0)
    {
        delete[] longImage;
        longImageCount = 0;
    }
        
}
