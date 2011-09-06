/*
 *  CameraTheosVideoInput.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/03/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "CameraTheosVideoInput.h"

#ifdef TARGET_WIN32

bool CameraTheosVideoInput::setup(int ID)
{
	_ID = ID;

	greyPixels = new unsigned char[camWidth * camHeight];
	rgbPixels = new unsigned char[camWidth * camHeight * 3];

	ofLog(OF_LOG_VERBOSE, "Camera: Initialising videoInput device " + ofToString(ID) + "(" + _grabber.getDeviceName(0) + ")");

	_initialised = _grabber.setupDevice(ID, camWidth, camHeight);
	
	return _initialised;
}

void CameraTheosVideoInput::videoSettings()
{
	_grabber.showSettingsWindow(ID);
}

void CameraTheosVideoInput::grab()
{
	//cout << ofGetElapsedTimef() << " - startgrab\n";
	_grabber.getPixels(ID, rgbPixels,true, true);

	for (int iPixel=0; iPixel<camWidth*camHeight; iPixel++)
	{
		greyPixels[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			greyPixels[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
	//cout << ofGetElapsedTimef() << " - endgrab\n";
}

void CameraTheosVideoInput::close()
{
	_grabber.stopDevice(ID);
}

#endif