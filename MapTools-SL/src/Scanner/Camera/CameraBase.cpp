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
_initialised(false)
{
	startThread(true, false);
    clearTimer();
}

CameraBase::~CameraBase()
{
	stopThread();
}

void CameraBase::clearTimer()
{
	lastTimerClear = ofGetElapsedTimeMillis();
}

bool CameraBase::hasWaited()
{
	return (ofGetElapsedTimeMillis() - lastTimerClear) > captureDelay;
}

bool CameraBase::capture(unsigned char*& pixels, bool forceWait)
{
	//cout << ofGetElapsedTimef() << " - startcap\n";

	bool waited;
	if (!forceWait)
		waited = hasWaited();
	else
	{
		//lock the thread
		while(!lock())
		{
			ofSleepMillis(1);
			cout << ".";
		}

		waited = hasWaited();

		grab();

		unlock();
	}

	pixels = greyPixels;

	//cout << ofGetElapsedTimef() << " - endcap\n";
	return waited;
}

void CameraBase::threadedFunction()
{
	while (isThreadRunning())
	{
		ofSleepMillis(10);
		if (lock())
		{
			if (_initialised)
				grab();
			unlock();
		}
	}
}