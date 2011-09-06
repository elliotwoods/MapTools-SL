/*
 *  CorrelateMain.h
 *  MapTools Structured Light
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#include "ofxCVgui.h"

#include "scrSelectBounds.h"

#include "CorrelateRays.h"

#include <fstream>
#include <iostream>

#define MAXPOINTS 10000000

class CorrelateMain
{
public:
	CorrelateMain();
	void			update();
	
	//
	
	scrGroupGrid	scrGridMain, scrGridData;
	scrWidgets		scrControl;
	scrFileSelect	scrFileSelection;
    scrSelectBounds scrWorldSpace;
	
protected:
	void			loadData();
	
	void			evaluate();
	
	void			save3DScan();
    void            addToImage();
    void            saveImage();
    void            clearImage();
	
	//
	
	wdgButton		*bangLoad, *bangEvaluate, *bangLoadCalibration;
	wdgButton		*bangSave3DScan, *bangAddToImage, *bangSaveImage, *bangClearImage;

	unsigned int	nPoints;
	bool			swapCameras;
    string          lastFilename;
	
	//Calibration
	CorrelateRays	calibration;
	vector<CamPoint>	dataCameraSpace;
	vector<ofVec3f>		dataWorldSpace;
	float			stereoIntersectThreshold;
	

	// projector index data
	unsigned short	projWidth, projHeight;
	unsigned int	dataset_iPX[MAXPOINTS];
	unsigned int	dataset_iPY[MAXPOINTS];
	
    
    // image output
    unsigned char * longImage;
    unsigned int	longImageCount;

	
};