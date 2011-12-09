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

bool CameraTheosVideoInput::init(int ID, int width, int height)
{
	bool success = _grabber.setupDevice(ID, width, height);
	_preview.allocate(width, height, GL_RGB);
	preview = &_preview;

	rgbPixels = new unsigned char[width*height*3];

	return success;
}

void CameraTheosVideoInput::videoSettings()
{
	_grabber.showSettingsWindow(ID);
}

void CameraTheosVideoInput::grab() {
	_grabber.getPixels(ID, rgbPixels, true, true);

	for (int iPixel=0; iPixel<getWidth()*getHeight(); iPixel++)
	{
		greyPixels[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			greyPixels[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
	//cout << ofGetElapsedTimef() << " - endgrab\n";
}

void CameraTheosVideoInput::grabPreview() {
	_grabber.getPixels(ID, rgbPixels, true, true);
	_preview.loadData(rgbPixels, getWidth(), getHeight(), GL_RGB);
}

void CameraTheosVideoInput::close()
{
	_grabber.stopDevice(ID);
}

#endif