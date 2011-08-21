/*
 *  CorrelateMain.h
 *  PC Encode
 *
 *  Created by Elliot Woods on 03/11/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#include "ofxCVgui.h"
#include "ofxPolyfit.h"

#include "scrSelectBounds.h"

#include <fstream>
#include <iostream>

#define MAXPOINTS 10000000

typedef double DataType;

class CorrelateMain
{
public:
	CorrelateMain();
	void			update();
	
	//
	
	scrGroupGrid	scrGridMain, scrGridData;
	scrWidgets		scrControl;
	scrFileSelect	scrFileSelection;
	scrPointCloud	scrInputPoints;
    scrSelectBounds scrTestCorrelate;
	
protected:
	void			loadData();	
	float			getDepthFromFilename(string filename);
	void			copyToInputScreen();
	
	void			runPolyfit();
	void			evaluate();
	
	void			save3DScan();
    void            addToImage();
    void            saveImage();
    void            clearImage();
	
	//
	
	wdgButton		*bangLoad, *bangCorrelate, *bangEvaluate, *bangLoadFit, *bangSaveFit;
	wdgButton		*bangSave3DScan, *bangAddToImage, *bangSaveImage, *bangClearImage;
	bool			newFormat;

	unsigned int	nCameras;
	unsigned int	nDatasets;
	unsigned int	nPoints;
	float			screenWidth, screenHeight;
	bool			swapCameras;
    string          lastFilename;
	

	//use same colour as input
	
	// projector index data
	unsigned short	projWidth, projHeight;
	unsigned int	dataset_iPX[MAXPOINTS];
	unsigned int	dataset_iPY[MAXPOINTS];
	
	// fit data
	ofxPolyFit_<DataType>	fit;
	float					polyOrder; //it's a float because of slider
	pfitDataSet<DataType>	dataSet;
	pfitDataSet<DataType>	evaluateSet;

    
    // image output
    unsigned char * longImage;
    unsigned int	longImageCount;

	
};