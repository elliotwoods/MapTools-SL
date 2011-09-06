/*
 *  PCManager.cpp
 *  MapTools Structured Light VS
 *
 *  Created by Elliot Woods on 26/10/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */


#include "PCManager.h"

#define FOREACH_CAMERA for (int iCam=0; iCam<nCameras; iCam++)

PCManager::PCManager() :
_scrProjectorMask(_texProjectorMask, "Projector mask"),
_wdgStartScan("Start scan"),
_wdgClear("Clear all (including mask)"),
_scrControls("Controls"),
_wdgDistance("Screen distance", screenDistance, 0, 1, 0.01,"meters"),
_wdgIFrame("iFrame",iFrame,0),
_wdgScanForever("Scan forever + add to p mask", scanForever)
{
	isInitialised=false;
	hasData=false;	
	state=0;
	iFrame=0;
	_firstFrame=false;
	
	//projector mask
	_charProjectorMask = new unsigned char[projPixelCount];
	_boolProjectorMask = new bool[projPixelCount];
	_texProjectorMask.allocate(projWidth, projHeight, GL_LUMINANCE);
	_texProjectorMask.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	screenDistance = 0.5;
	scanForever = false;

	//interface
	_scrControls.push(&_wdgStartScan);
	_scrControls.push(&_wdgClear);
	_scrControls.push(&_wdgDistance);
	_scrControls.push(&_wdgIFrame);
	_scrControls.push(&_wdgScanForever);
}

PCManager::~PCManager()
{
    delete[] _charProjectorMask;
    delete[] _boolProjectorMask;
    
	if (isInitialised)
		close();
	
}

void PCManager::setup()
{
	
	if (!isInitialised)
	{
        
		//initialise payload
		Payload::Pointer = new PayloadBinary();
		Payload::Pointer->setup();
		
		//instantiate the encoder
		_encoder = new PCEncode(_boolProjectorMask);
		
		//instantiate cameras, decoders
		FOREACH_CAMERA
		{
			#ifdef PCENCODE_CAM_DEFAULT
			_camera.push_back(new CameraDefault());
			#endif
			
			#ifdef PCENCODE_CAM_VIDEO_INPUT
			_camera.push_back(new CameraTheosVideoInput());
			#endif
			
			#ifdef PCENCODE_CAM_EDSDK
			_camera.push_back(new CameraEdsdk());
			#endif
			

			_decoder.push_back(new PCDecode(*_camera[iCam], _boolProjectorMask));
			
			if (!_camera[iCam]->setup(camIDs[iCam], camWidth, camHeight, captureDelay))
				std::exit(iCam);
		}
		
		//instantiate logger
		_logger = new PCLogger(_encoder, &_decoder);
		
		//clear all data
		clearProjectorMask();
		
		isInitialised=true;
	} else
		ofLog(OF_LOG_ERROR, "PCManager: Can't run setup() more than once");
}

void PCManager::update()
{

	//////////////////////////
	// CHECK INPUTS
	//////////////////////////
	if (_wdgClear.getBang())
		clear(true);
	if (_wdgStartScan.getBang())
		start();
	//////////////////////////	
	
	if (_firstFrame) {
		writeFrame();
		_firstFrame=false;
		return;
	}
	
	//////////////////////////
	// CAPTURE
	//////////////////////////
	//if we're scanning,
	//then use scanning capture (allow camera to wait for new frames)
	//else preview capture
	FOREACH_CAMERA {
		if (state==0)
			_camera[iCam]->updatePreview();
		else if (!_firstFrame)
			_camera[iCam]->capture();
	
		_decoder[iCam]->update();
	}
	//

	//check that we've got a fresh frame.
	//for a dslr this means have we downloaded a frame from the camera
	//for a usb cam, it means have we waited long enough to clear the buffer
	bool frameReady=true;
	FOREACH_CAMERA
		frameReady &= _camera[iCam]->isFrameNew();
	if (!frameReady)
		return;

    //
	//////////////////////////

	
    
	//////////////////////////
	// SCANNING STEPS
	//////////////////////////
	//if it's the first frame
	//then we need to write before reading
	if (state>0 && !_firstFrame)
	{
		readFrame();
		
		advanceFrame();
	}
	
	if (state>0) //if we are (still) scanning
		writeFrame();
    //
	//////////////////////////
}

void PCManager::close()
{
	for (int i=0; i<nCameras; i++)
	{
		delete _decoder[i];
	}
	
	delete _logger;
	delete _encoder;
}

////////////////////////////////////////////////////////////

void PCManager::videoSettings()
{
	FOREACH_CAMERA
		_camera[iCam]->videoSettings();
}

////////////////////////////////////////////////////////////
void PCManager::calibrate()
{
	FOREACH_CAMERA
		_decoder[iCam]->initCalibration();
	
	state = STATE_CALIBRATING;
	iFrame=0;
	_firstFrame=true;	
	
	_wdgIFrame.setMax(Payload::Pointer->calibrateFrameCount);
}

void PCManager::start()
{
	clear(false);
	
	state = STATE_SCANNING;
	iFrame=0;
	_firstFrame=true;
	
	_wdgIFrame.setMax(Payload::Pointer->totalFrames);
}

void PCManager::stop()
{
	state = STATE_STANDBY;
}

void PCManager::clear(bool clearMask)
{
	FOREACH_CAMERA
		_decoder[iCam]->clear(clearMask);
	
	if (clearMask)
		clearProjectorMask();
}

void PCManager::save(string filenameBase)
{
	_logger->save(filenameBase);
}
////////////////////////////////////////////////////////////

void PCManager::readFrame()
{
	if (state==STATE_CALIBRATING)
		FOREACH_CAMERA
			_decoder[iCam]->addCalibrationFrame(iFrame-1);
	
	else if(state==STATE_SCANNING)
	{
		
		FOREACH_CAMERA
			_decoder[iCam]->addScanFrame(Payload::Pointer->iScanInterleaveFrame(iFrame), Payload::Pointer->iInterleave(iFrame));
		
		ofLog(OF_LOG_VERBOSE, "PCManager: Read interleave frame " + ofToString(Payload::Pointer->iScanInterleaveFrame(iFrame), 0));

		//check whether we're changing interleaves
		if (Payload::Pointer->iInterleave(iFrame) !=
			Payload::Pointer->iInterleave(iFrame+1))
		{
			//if so, calculate for this interleave
			FOREACH_CAMERA
                _decoder[iCam]->calcInterleave(Payload::Pointer->iInterleave(iFrame));
			
			ofLog(OF_LOG_VERBOSE, "PCManager: Calc interleave " + 
				  ofToString(Payload::Pointer->iInterleave(iFrame), 0));
		}
	}
}

void PCManager::advanceFrame()
{
//	ofLog(OF_LOG_VERBOSE, "PCManager: Advance frame from " + ofToString(iFrame, 0));
	if (state>0)
		iFrame++;
	
	//check whether we've hit the end
	switch (state) {
		case STATE_CALIBRATING:
			if (iFrame >= Payload::Pointer->calibrateFrameCount)
			{
				FOREACH_CAMERA
					_decoder[iCam]->calcThreshold();
				stop();
				ofLog(OF_LOG_VERBOSE, "PCManager: end calibrate");
				if (scanForever)
					start();
			}
			break;
			
		case STATE_SCANNING:
			if (iFrame >= Payload::Pointer->totalFrames)
			{
				stop();
				if (scanForever)
					updateProjectorMask();
				ofLog(OF_LOG_VERBOSE, "PCManager: end scan");
				if (scanForever)
					calibrate();
			}
			break;
	}
//	ofLog(OF_LOG_VERBOSE, "PCManager: Advance frame to " + ofToString(iFrame, 0));
}

void PCManager::writeFrame()
{
	if (state==STATE_CALIBRATING)
		_encoder->updateCalibrationFrame(iFrame);
	
	else if(state==STATE_SCANNING)
	{
		_encoder->updateScanFrame(Payload::Pointer->iScanInterleaveFrame(iFrame),
								  Payload::Pointer->iInterleave(iFrame));

		ofLog(OF_LOG_VERBOSE, "PCManager: Write interleave frame " + ofToString(Payload::Pointer->iScanInterleaveFrame(iFrame), 0));
	}	
	
	if (state>0)
		FOREACH_CAMERA
			_camera[iCam]->clearTimer();
}

void PCManager::clearProjectorMask()
{
	for (int iPP=0; iPP<projPixelCount; iPP++)
	{
		_boolProjectorMask[iPP]=true;
		_charProjectorMask[iPP]=255;
	}
	_texProjectorMask.loadData(_charProjectorMask,projWidth,projHeight,GL_LUMINANCE);
	
	FOREACH_CAMERA
		_decoder[iCam]->clear(true);
	
}

void PCManager::updateProjectorMask()
{
	bool hasFinds;
	
	for (int iPP=0; iPP<projPixelCount; iPP++)
	{
		hasFinds = true;
		FOREACH_CAMERA
			hasFinds &= _decoder[iCam]->projPixels[iPP]->nFinds>0;
		
		_boolProjectorMask[iPP] = !hasFinds;
		_charProjectorMask[iPP] = (hasFinds ? 255 : 0);
	}
	
	_texProjectorMask.loadData(_charProjectorMask, projWidth, projHeight, GL_LUMINANCE);
}