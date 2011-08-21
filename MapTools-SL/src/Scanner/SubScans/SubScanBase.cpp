//
//  SubScanBase.cpp
//  PC Encode
//
//  Created by Elliot Woods on 13/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanBase.h"

SubScanBase::SubScanBase(unsigned char *& cameraGrey, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan) : 
_cameraGrey(cameraGrey),
_projectorScanMask(projectorMask),
_histThrehsoldRange(histThresholdRange),
_iSubScan(iSubScan),
screensThreshold("Calibration " + indexString()),
screensFrameData("Frame data  " + indexString())
{
    _cameraBinary = new bool[camPixelCount];
    _cameraBinaryPreview = new unsigned char[camPixelCount];
    _calibrateMin = new unsigned char[camPixelCount];
    _calibrateMax = new unsigned char[camPixelCount];
    
    _calibrateThresholdMid = new unsigned char[camPixelCount];
    _calibrateThresholdRange = new unsigned char[camPixelCount];
    
    _calibrateThresholdMask = new bool[camPixelCount];
    _calibrateThresholdMasked = new unsigned char[camPixelCount];
    
    _projectorSubScanMask = new bool[projPixelCount];
    
    _subFrameData = new unsigned long[camPixelCount];
    _subFrameDataPreview = new unsigned char[camPixelCount];
    _subFrameParity = new unsigned long[camPixelCount];
    _subFrameValid = new bool[camPixelCount];
    
    //GRAPHICS
    _texCameraBinary.allocate(camWidth, camHeight, GL_LUMINANCE);
    _texThresholdMasked.allocate(camWidth, camHeight, GL_LUMINANCE);
    _texSubFrameData.allocate(camWidth, camHeight, GL_LUMINANCE);
    
    ////////////////////////////////
    //  SCREENS
    ////////////////////////////////
    //
    _scrThreshold = new scrTexture(_texThresholdMasked, "Threshold");
    screensThreshold.push(_scrThreshold);
    
	_scrBinary = new scrTexture(_texCameraBinary, "Binary image");
    screensThreshold.push(_scrBinary);
    
    _scrSubFrameData = new scrTexture(_texSubFrameData, "Data");
    screensFrameData.push(_scrSubFrameData);
    //
    ////////////////////////////////    
}

SubScanBase::~SubScanBase()
{
    delete[] _cameraBinary;
    delete[] _cameraBinaryPreview;
    
    delete[] _calibrateMin;
    delete[] _calibrateMax;
    
    delete[] _calibrateThresholdMid;
    delete[] _calibrateThresholdRange;
    
    delete[] _projectorSubScanMask;
    
    delete[] _subFrameData;
    delete[] _subFrameDataPreview;
    delete[] _subFrameParity;
    delete[] _subFrameValid;
    
    delete[] _scrThreshold;
    delete[] _scrBinary;
    delete[] _scrSubFrameData;
}

void SubScanBase::clearData()
{
    for (int i=0; i<camPixelCount; i++)
    {
        _subFrameData[i] = 0;
        _subFrameParity[i] = 0;
    }
}

void SubScanBase::initCalibration()
{
    for (int i=0; i<camPixelCount; i++)
	{
		_calibrateMax[i]=0;
		_calibrateMin[i]=255;
	}
}

void SubScanBase::addCalibrationHigh()
{
	for (int i=0; i<camPixelCount; i++)
        if (_calibrateMax[i] < _cameraGrey[i])
            _calibrateMax[i] = _cameraGrey[i];
}

void SubScanBase::addCalibrationLow()
{
	for (int i=0; i<camPixelCount; i++)
        if (_calibrateMin[i] > _cameraGrey[i])
            _calibrateMin[i] = _cameraGrey[i];
}

void SubScanBase::calcCalibration()
{
    unsigned char rangeMinMax;
    
	for (int i=0; i<camPixelCount; i++)
	{
        if (_calibrateMax[i] < _calibrateMin[i])
        {
            _calibrateThresholdMid[i] = 255;
            _calibrateThresholdRange[i] = 0;
            
            continue;
        }
        
        _calibrateThresholdRange[i] = int(_calibrateMax[i]) - int(_calibrateMin[i]);
		
        _calibrateThresholdMid[i] = (unsigned char)(float(_calibrateThresholdRange[i]) * thresholdPercentile) + _calibrateMin[i];
		
		_histThrehsoldRange.add(_calibrateThresholdRange[i]);        
		
	}
	
	renderThresholdMasked();
}

void SubScanBase::calcBinary()
{
    //This function performs thresholding
    //PCDecode performs the camera capture then calls this
    
    for (int i=0; i<camPixelCount; i++)
	{
		_cameraBinary[i] = _cameraGrey[i] > _calibrateThresholdMid[i];
		_cameraBinaryPreview[i] = (_cameraBinary[i] && _calibrateThresholdMask[i]) * 255;
	}
	
	_texCameraBinary.loadData(_cameraBinaryPreview, camWidth, camHeight, GL_LUMINANCE);    
    
}

void SubScanBase::updateThresholdSelection(int selection)
{
    _calibrateRangeQualifier = selection;
    renderThresholdMasked();
}

void SubScanBase::addScanFrame(int iSubScanFrame)
{
	if (iSubScanFrame==0)
		clearData();
	
	int nScanFramesPerInterleave = Payload::Pointer->totalFramesPerInterleave;
	unsigned long binaryFrame;
	
    //get iScanFrame or iErrorCheckFrame respectively
	binaryFrame = 1 << (iSubScanFrame % Payload::Pointer->dataFramesPerInterleave);
    
	for (int i=0; i<camPixelCount; i++)
		if (_calibrateThresholdMask[i])
		{
			//store the values that we receive
            
            //is it a data frame or error check frame?
			if (iSubScanFrame < Payload::Pointer->dataFramesPerInterleave)                
				_subFrameData[i] |= _cameraBinary[i] * binaryFrame;
            
			else
				_subFrameParity[i] |= _cameraBinary[i] * binaryFrame;
			
			_subFrameDataPreview[i] = float(_subFrameData[i])/ float(1 << nScanFramesPerInterleave) * 255;
			
		} else {
			_subFrameData[i]=0;
			_subFrameDataPreview[i]=0;
		}
	
    //if this is the last scan (data+parity) frame 
	if(iSubScanFrame == Payload::Pointer->totalFramesPerInterleave-1)
		checkParity();
	
	_texSubFrameData.loadData(_subFrameDataPreview, camWidth, camHeight, GL_LUMINANCE);
}

void SubScanBase::checkParity()
{
    //if no error check, set this all to true
    if (errorBits == 0)
    {
        memset(_subFrameValid, true, camPixelCount);
        return;
    }
    
    unsigned long iSubScanPixel;
	unsigned long sentParity;

	
	for (unsigned long i=0; i<camPixelCount; i++)
	{
        
		iSubScanPixel = Payload::Pointer->dataInverse[_subFrameData[i]];
		
		if (iSubScanPixel < SubScanPixelCount && iSubScanPixel>0)
		{
			
			sentParity = Payload::Pointer->errorCheck[iSubScanPixel];
			
			_subFrameValid[i] = (sentParity == _subFrameParity[i]);
			
		} else
			_subFrameValid[i] = false;		
	}
}

void SubScanBase::calc(vector<PCPixel*> &camPixels, vector<PCPixel*> &projPixels)
{
    unsigned long iCamPixelX, iCamPixelY, iCamPixel;
	float xCamX, xCamY;
    
	unsigned long iProjectorSubScanPixel, iProjectorSubScanPixelX, iProjectorSubScanPixelY;
	
	unsigned long iProjectorPixel, iProjectorPixelX, iProjectorPixelY;
    
	float xProjectorX, xProjectorY;
    
	bool isValid;
	
	for (iCamPixelX = 0; iCamPixelX < camWidth; iCamPixelX++)
		for (iCamPixelY=0; iCamPixelY < camHeight; iCamPixelY++) {
			
			iCamPixel = iCamPixelY * camWidth + iCamPixelX;
			
			if (_calibrateThresholdMask[iCamPixel]) // DOES THIS PIXEL FIT WITHIN SELECTED THRESHOLD MIN RANGE
			{
				// calculate camera space floatwise
				xCamX = float(iCamPixelX)/float(camWidth-1);
				xCamY = float(iCamPixelY)/float(camHeight-1);			
				
				//use payload to decode reading into position indicies
                //returns false if value outside ranges
				isValid = Payload::Pointer->decode(_subFrameData[iCamPixel], iProjectorSubScanPixelX, iProjectorSubScanPixelY, SubScanWidth, SubScanHeight);
                
                //check we passed parity check
                isValid &= _subFrameValid[iCamPixel];
                
                if (!isValid)
                    continue;
                
				// assign found value indexwise				
                iProjectorPixelX = idxPiSx[iProjectorSubScanPixelX];
                iProjectorPixelY = idxPiSy[iProjectorSubScanPixelY];
                
				iProjectorPixel = iProjectorPixelY * projWidth + iProjectorPixelX;
				
				xProjectorX = float(iProjectorPixelX)/float(projWidth);
				xProjectorY = float(iProjectorPixelY)/float(projHeight);
				
				//check whether is within current projector mask
				if (!_projectorScanMask[iProjectorPixel])
                    continue;
                
                projPixels.at(iProjectorPixel)->addFind(iCamPixel, xCamX, xCamY);
                
                camPixels.at(iCamPixel)->addFind(iProjectorPixel, xProjectorX, xProjectorY);
			}
		}
}

void SubScanBase::allocateLookups()
{
    SubScanPixelCount = SubScanWidth * SubScanHeight;
    
    idxSiPx = new unsigned long[projWidth];
    idxSiPy = new unsigned long[projHeight];
    idxSiP = new unsigned long[projPixelCount];
    
    idxPiSx = new unsigned long[SubScanWidth];
    idxPiSy = new unsigned long[SubScanWidth];
    idxPiS = new unsigned long[SubScanPixelCount];
}

void SubScanBase::renderLookups()
{
    memset(idxSiPx, 0, sizeof(unsigned long) * projWidth);
    memset(idxSiPy, 0, sizeof(unsigned long) * projHeight);
    memset(idxSiP, 0, sizeof(unsigned long) * projPixelCount);
    
    //i, j, idx in SubScan space
    //i_dash, j_dash, idx_dash in Projector space
    
    unsigned long i, j, idx;
    unsigned long i_dash, j_dash, idx_dash;
    
    for (i = 0; i < SubScanWidth; i++)
    {
        i_dash = lookupPiSx(i);
        idxPiSx[i] = i_dash;
        idxSiPx[i_dash] = i;
    }
    
    for (j = 0; j < SubScanHeight; j++)
    {
        j_dash = lookupPiSy(j);
        idxPiSy[j] = j_dash;
        idxSiPy[j_dash] = j;
    }
    
    for (idx = 0; idx < SubScanPixelCount; idx++)
    {
        i = idx % SubScanWidth;
        j = idx / SubScanWidth;
        
        idx_dash = idxPiSx[i] + idxPiSy[j] * projWidth;
        idxPiS[idx] = idx_dash;
        idxSiP[idx_dash] = idx;
    }
    
}

void SubScanBase::renderThresholdMasked()
{
    for (int i=0; i<camPixelCount; i++)
    {
        _calibrateThresholdMask[i] = _calibrateThresholdRange[i] > _calibrateRangeQualifier;
        
        _calibrateThresholdMasked[i] = (_calibrateThresholdMask[i] ? _calibrateThresholdMid[i] : 0);
    }
    
    _texThresholdMasked.loadData(_calibrateThresholdMasked, camWidth, camHeight, GL_LUMINANCE);
}

void SubScanBase::applyThresholdToCamera()
{
    for (int i=0; i<camPixelCount; i++)
    {
        _cameraBinary[i] = _cameraGrey[i] >= _calibrateThresholdMid[i];
        _cameraBinaryPreview[i] = (_cameraBinary[i] ? 255 : 0);
    }
}

string SubScanBase::indexString()
{
    return "[" + ofToString(_iSubScan) + "]";
}