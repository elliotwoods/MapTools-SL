#pragma once
//
//  SubScanBase.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 13/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

////////////////////////
// NOTES FOR OVERRIDING
// 
// Constructor should render idxIiP, idxPiI
////////////////////////

#include "ofMain.h"
#include "PCConfig.h"
#include "ofxCVGui.h"
#include "PayloadBase.h"
#include "PCPixel.h"
#include "CameraBase.h"

#include <sstream>

class SubScanBase : public PCConfig
{
    public:
        SubScanBase(CameraBase &camera, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan);
        ~SubScanBase();
    
        void    clearData();    

        void    initCalibration();    
        void    addCalibrationHigh();
        void    addCalibrationLow();
        void    calcCalibration();
    
        void    updateThresholdSelection(int selection);
        void    addScanFrame(int iSubScanFrame);
        
        void    checkParity();    
        void    calc(vector<PCPixel*> &projPixels, vector<PCPixel*> &camPixels);
    
        scrGroupTabbed    screensThreshold;
        scrGroupTabbed    screensFrameData;
    
    protected:
        
        //////////////////////////////////
        // VIRTUAL METHODS
        //////////////////////////////////
        //
        virtual unsigned long lookupPiSx(unsigned long iX) = 0;
        virtual unsigned long lookupPiSy(unsigned long iY) = 0;
        //
        //////////////////////////////////
        
		void    calcBinary();
	
        void    allocateLookups();
        void    renderLookups();
    
        void    renderThresholdMasked();
    
        //this mask and the lookup function
        //define the subscan
        bool *              _projectorSubScanMask;
        //
    
        bool *&             _projectorScanMask;
    
        
        string  indexString();
    
        //////////////////////////////////
        // SUBSCAN PROPERTIES
        //////////////////////////////////
        //
        int     _iSubScan;
    
        unsigned long SubScanWidth;
        unsigned long SubScanHeight;
        unsigned long SubScanPixelCount;
    
        unsigned long *idxSiP; // Interleave in Projector
        unsigned long *idxSiPx, *idxSiPy;
        
        unsigned long *idxPiS; // Projector in Interleave
        unsigned long *idxPiSx, *idxPiSy;   
        //
        //////////////////////////////////

        
    
        //////////////////////////////////
        // CAMERA
        //////////////////////////////////
        //
		ofPixels&			_cameraPixels;
        bool*               _cameraBinary;
        unsigned char *     _cameraBinaryPreview;
        //////////////////////////////////    
    
    
    
        //////////////////////////////////
        // THRESHOLD
        //////////////////////////////////
        //
        unsigned char       _calibrateRangeQualifier;
        Histogram &         _histThrehsoldRange;
    
        unsigned char *     _calibrateMin;
        unsigned char *     _calibrateMax;
    
        unsigned char *     _calibrateThresholdMid;
        unsigned char *     _calibrateThresholdRange;
    
        bool *              _calibrateThresholdMask;
        unsigned char *     _calibrateThresholdMasked;
        //
        //////////////////////////////////

    
    
        ///////////////////////////
        // FRAME DATA
        ///////////////////////////
        //
        unsigned long *	_subFrameData;
        unsigned char * _subFrameDataPreview;

        unsigned long *	_subFrameParity;
        bool *			_subFrameValid;
        //
        ///////////////////////////    
    
    
    
        //////////////////////////////////
        // GRAPHICS
        //////////////////////////////////
        //
        ofTexture	_texCameraBinary;
        ofTexture	_texSubFrameData;
        ofTexture	_texThresholdMasked;
    
        scrDraw2D	*_scrThreshold;
        scrDraw2D	*_scrBinary;
        scrDraw2D	*_scrSubFrameData;
    
        Histogram	*_histThresholdRange;
        //
        //////////////////////////////////
    
};