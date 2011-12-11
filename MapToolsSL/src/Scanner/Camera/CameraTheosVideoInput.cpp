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

	rgbPixels.allocate(width, height, OF_IMAGE_COLOR);
	this->startThread(true, false);

	return success;
}

void CameraTheosVideoInput::videoSettings()
{
	_grabber.showSettingsWindow(ID);
}

void CameraTheosVideoInput::grab() {
	lock();
	fresh = false;
	unlock();
	
	while(!fresh)
		ofSleepMillis(1);

	lock();
	for (int iPixel=0; iPixel<getWidth()*getHeight(); iPixel++)
	{
		greyPixels[iPixel] = 0;
		for (int iColour=0; iColour<3; iColour++)
			greyPixels[iPixel] += rgbPixels[iPixel*3+iColour]/3;
	}
	unlock();
}

void CameraTheosVideoInput::grabPreview() {
	//doesn't matter if we get tearing in the preview
	//so we don't double buffer here
	_preview.loadData(rgbPixels.getPixels(), getWidth(), getHeight(), GL_RGB);
}

void CameraTheosVideoInput::threadedFunction() {
	while (threadRunning)
	{
		lock();
		_grabber.getPixels(ID, rgbPixels.getPixels(), true, true);
		fresh = true;
		unlock();
		ofSleepMillis(1);
	}
}

void CameraTheosVideoInput::close()
{
	stopThread();
	_grabber.stopDevice(ID);
}

#endif