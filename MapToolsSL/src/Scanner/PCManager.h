#pragma once

/*
 *  PCManager.h
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */


#include "ofMain.h"

#include "PCEncode.h"
#include "PCLogger.h"
#include "PCDecode.h"

#include "SubScanSet.h"
#include "SubScanBasic.h"

#include "PayloadGraycode.h"
#include "PayloadBinary.h"

#include "ofxCVgui.h"

#define STATE_STANDBY				0
#define STATE_CALIBRATING			1
#define STATE_SCANNING				2

/////////////////////////////////////
// CAMERAS
/////////////////////////////////////
//
#ifdef TARGET_OSX
	#define PCENCODE_CAM_EDSDK
#else
	#define PCENCODE_CAM_VIDEO_INPUT
#endif
//
///////
//
#ifdef PCENCODE_CAM_DEFAULT
	#include "CameraDefault.h"
#endif
//
#ifdef PCENCODE_CAM_VIDEO_INPUT
	#include "CameraTheosVideoInput.h"
#endif
//
#ifdef PCENCODE_CAM_EDSDK
	#include "CameraEdsdk.h"
#endif
//
/////////////////////////////////////

class PCManager : public PCConfig 
{
public:
	PCManager();
	~PCManager();
	
	void	setup();
	void	update();
	void	close();
	
	void	videoSettings();

	void	calibrate();
	void	start();
	void	stop(); //force early stop

	void	clear(bool clearMask=false);
	void	save(string filenameBase);
	
	bool	isInitialised;
	bool	hasData;
	bool	scanForever;
	int		state;
	unsigned int		iFrame;

	PCEncode			*_encoder;
	vector<PCDecode*>	_decoder;
	
	PCLogger			*_logger;
    
    SubScanSet          *_subScans;
	
	vector<CameraBase*>		_camera;
		
	//interface
	scrTexture				_scrProjectorMask;
	scrWidgets				_scrControls;

	//cal vars
	float				screenDistance;

protected:
	void	readFrame();
	void	writeFrame();
	void	advanceFrame();

	bool	_firstFrame;
	vector<bool>	_waitingForFrame;
	
	void					updateProjectorMask();
	void					clearProjectorMask();

	//interface
	wdgButton			_wdgStartScan;
	wdgButton			_wdgClear;
	wdgSlider			_wdgDistance;
	wdgCounter			_wdgIFrame;
	wdgButton			_wdgScanForever;

	//mask
	ofTexture				_texProjectorMask;
	bool *					_boolProjectorMask;
	unsigned char *			_charProjectorMask;
	
};
