#ifndef _PCDecode
#define _PCDecode
/*
 *  PCDecode.h
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */

#include "CameraBase.h"
#include "PCconstants.h"
#include "PCConfig.h"

#include "ofMain.h"

#include "PCEncode.h"
#include "ofxKCScreensGUI.h"

#include "PCPixel.h"

#include "PayloadBase.h"

#include "SubScanSet.h"

#include <algorithm>

class PCDecode : public PCConfig 
{
	public:
		PCDecode(CameraBase *camera, bool *boolProjectorMask);
		~PCDecode();
	
		bool capture(bool needFreshFrame=false);
	
		void updateCameraSpacePreview();
		void updateProjectorSpacePreview();
	
		void initCalibration();
	
		void addCalibrationFrame(int iSubScan);
		void addScanFrame(int iSubScanFrame, int iSubScan);
		
		void calcThreshold();
		void calcInterleave(int iSubScan);
		
		void clear(bool clearFinds);
		void clearSubScan(int iSubScan);
	
		int	getNFinds(int iProjectorPixel);
		void getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, int &iLastFoundCameraPixel);
		void getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, float &CamSigmaX, float &CamSigmaY, int &iLastFoundCameraPixel);

		unsigned char *			_charCameraSpacePreview;
		unsigned char *			_charProjectorSpacePreview;
	
		scrBase					*_scrProjectorSpace, *_scrCameraSpace;
		scrBase					*_scrCameraNFinds, *_scrProjectorNFinds;
        scrTexture              _scrCamera;
    
		scrHistograms			*_scrHistograms;
    
        scrGroupBase            *_scrSubScans;
	
		std::vector<PCPixel*>	projPixels;
		std::vector<PCPixel*>	camPixels;
	
	private: 
		void					updateThresholdSelection(int &iSelectionClass);
		void					updateSpacePreview(int width, int height, unsigned char data[], ofTexture &texture, std::vector<PCPixel*> &foundPixels);
		
		void					moveSendCursor(ofPoint &ptCursorPosition);
	
		CameraBase				*_camera;
		
		ofTexture				_texCamera;
		ofTexture				_texCameraSpacePreview, _texProjectorSpacePreview;
		ofTexture				_texCameraSpaceNFinds, _texProjectorSpaceNFinds;
		
		Histogram	 			_histThresholdRange;
		Histogram	 			_histNFinds;
		
		unsigned char *			_charCamera;
        unsigned char           _charCalibrateRangeThreshold;
    
		unsigned char *			_charCameraSpaceNFinds, *_charProjectorSpaceNFinds;

    
    
        ///////////////////////////
        // FRAME DATA
        ///////////////////////////
        //
        SubScanSet              _subScanSet;
        //
        ///////////////////////////    
        
        //mask defined by PCManager
    	bool *					_boolProjectorMask;

	
};

#endif