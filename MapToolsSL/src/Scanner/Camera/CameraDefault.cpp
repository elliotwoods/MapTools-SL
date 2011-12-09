//
//  CameraDefault.cpp
//  MapTools Structured Light
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CameraDefault.h"

void CameraDefault::videoSettings(){
    _grabber.videoSettings();
}

void CameraDefault::close(){
    _grabber.close();
}

bool CameraDefault::init(int ID, int width, int height)
{
    bool success = _grabber.initGrabber(width, height, true);
	_grabber.listDevices();
	preview = &_grabber.getTextureReference();
	
	return success;
}

void CameraDefault::grab(){
    _grabber.grabFrame();
    
    unsigned char * rgbPixels = _grabber.getPixels();
	
	
	//preview is same resolution as full frame, so we can use getWidth/Height here
	for (int iPixel=0; iPixel<getWidth()*getHeight(); iPixel++)
	{
		greyPixels[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			greyPixels[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
}

void CameraDefault::grabPreview(){
	_grabber.update();
	preview = &_grabber.getTextureReference();
}