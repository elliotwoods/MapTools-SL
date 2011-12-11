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

#include "scrScanSet.h"

#include "CorrelateRays.h"
#include "ScanSet.h"

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
    scrScanSet		*scrWorldSpace;
	
protected:
	void			loadData();
	
	void			evaluate();
	
	void			saveScan();
	void			saveMap();
    void            addToImage();
    void            saveImage();
    void            clearImage();
	
	//
	
	wdgButton		*bangLoad, *bangEvaluate, *bangLoadCalibration;
	wdgButton		*bangSave3DScan, *bangAddToImage, *bangSaveImage, *bangClearImage;
	wdgButton		*bangSaveMap;

	unsigned int	nPoints;
	bool			swapCameras;
    string          lastFilename;
	
	//Calibration
	CorrelateRays	calibration;
	vector<CamPoint>	dataCameraSpace;
	float			stereoIntersectThreshold;
	

	// projector index data
	unsigned short	projWidth, projHeight;
	unsigned int	dataset_iPX[MAXPOINTS];
	unsigned int	dataset_iPY[MAXPOINTS];
	
	ScanSet			scanSet;
    
    // image output
	ofFloatPixels	longImage;
    unsigned int	longImageCount;
};