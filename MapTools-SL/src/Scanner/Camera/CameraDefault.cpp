//
//  CameraDefault.cpp
//  PC Encode
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CameraDefault.h"

bool CameraDefault::setup(int ID)
{
	_ID = ID;
    
	greyPixels = new unsigned char[camWidth * camHeight];
    
	ofLog(OF_LOG_VERBOSE, "Camera: Initialising ofVideoGrabber device " + ofToString(ID));
    
    _initialised = _grabber.initGrabber(camWidth, camHeight, false);
	
	return _initialised;    
}

void CameraDefault::videoSettings(){
    _grabber.videoSettings();
}

void CameraDefault::close(){
    _grabber.close();
}

void CameraDefault::grab(){
    _grabber.grabFrame();
    
    unsigned char * rgbPixels = _grabber.getPixels();
        
	for (int iPixel=0; iPixel<camWidth*camHeight; iPixel++)
	{
		greyPixels[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			greyPixels[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
}