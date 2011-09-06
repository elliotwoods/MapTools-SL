/*
 *  CameraBase.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/03/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */
#include "CameraBase.h"

CameraBase::CameraBase() :
ID(_ID),
_initialised(false),
preview(0) {
    clearTimer();
}

CameraBase::~CameraBase() {
}

bool CameraBase::setup(int ID, int width, int height, float delay) {
	ofLogVerbose() << "Camera: Initialising ofVideoGrabber device " << ID;
	
	if (init(ID, width, height)) {
		greyPixels.allocate(width, height, OF_PIXELS_MONO);
		_initialised = true;
		_ID = ID;
	} else {
		ofLogError() << "Failed to intitialise camera " << ID;
		_initialised = false;
		_ID = -1;
	}
	
	this->delay = delay;
	
	return _initialised;
}

void CameraBase::updatePreview() {
	grabPreview();
}

void CameraBase::capture() {
	grab();
}

unsigned char * CameraBase::getPixels() {
	return greyPixels.getPixels();
}

ofPixels& CameraBase::getPixelsRef() {
	return greyPixels;
}

void CameraBase::draw(float x, float y) {
	if (preview != 0)
		preview->draw(x,y);
}

void CameraBase::draw(float x, float y, float w, float h) {
	if (preview != 0)
		preview->draw(x,y,w,h);
}

float CameraBase::getWidth() {
	if (preview != 0)
		return preview->getWidth();
	else
		return 0;
}

float CameraBase::getHeight() {
	if (preview != 0)
		return preview->getHeight();
	else
		return 0;
}

void CameraBase::clearTimer()
{
	lastTimerClear = ofGetElapsedTimeMillis();
}

bool CameraBase::hasWaited()
{
	return (ofGetElapsedTimeMillis() - lastTimerClear) > delay;
}
