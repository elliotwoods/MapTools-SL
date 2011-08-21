/*
 *  PCDecode.cpp
 *  PCDecode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */


#include "PCDecode.h"

PCDecode::PCDecode(CameraBase *camera, bool *boolProjectorMask) :
_histThresholdRange("Threshold range threshold (subscans)", 256),
_histNFinds("Camera pixels found per projector pixel.", projPixelCount),
_scrCamera(_texCamera, "Camera image")
{		
	_camera = camera;
	
	_boolProjectorMask = boolProjectorMask;
    
    _charCalibrateRangeThreshold = 50;
	
	///////////////////////////////////////
	// TEXTURE CHAR DATA
	///////////////////////////////////////	
	_charCameraSpacePreview = new unsigned char[projPixelCount*3];
	_charProjectorSpacePreview = new unsigned char[camPixelCount*3];

	_charCameraSpaceNFinds = new unsigned char[camPixelCount];
	_charProjectorSpaceNFinds = new unsigned char[projPixelCount];
	///////////////////////////////////////
	
	
	///////////////////////////////////////
	// TEXTURES
	///////////////////////////////////////
	_texCamera.allocate(camWidth, camHeight, GL_LUMINANCE);
	
	_texCameraSpacePreview.allocate(projWidth, projHeight, GL_RGB);
	_texProjectorSpacePreview.allocate(camWidth, camHeight, GL_RGB);
	_texCameraSpaceNFinds.allocate(camWidth, camHeight, GL_LUMINANCE);
	_texProjectorSpaceNFinds.allocate(projWidth, projHeight, GL_LUMINANCE);
	_texProjectorSpaceNFinds.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	///////////////////////////////////////

	
	///////////////////////////////////////
	// PIXEL FINDS
	///////////////////////////////////////
	
	ofLog(OF_LOG_VERBOSE, "PCDecode: Initialising " + ofToString(projPixelCount) +
		" projector pixels, each at " + ofToString((int)sizeof(PCPixelMeans)) + 
		" bytes , total = " + ofToString((float)sizeof(PCPixelMeans)*projPixelCount/(1024.0f*1024.0f),2) + "MB");
	for (int iPixel=0; iPixel<projPixelCount; iPixel++)
		projPixels.push_back(new PCPixelMeans());
	
	ofLog(OF_LOG_VERBOSE, "PCDecode: Initialising " + ofToString(camPixelCount) + 
		" camera pixels, each at " + ofToString((int)sizeof(PCPixelMeans)) + 
		" bytes , total = " + ofToString((float)sizeof(PCPixelMeans)*camPixelCount/(1024.0f*1024.0f),2) + "MB");
	for (int iPixel=0; iPixel<camPixelCount; iPixel++)
		camPixels.push_back(new PCPixelMeans());
	///////////////////////////////////////
	
	
	///////////////////////////////////////
	// HISTOGRAMS
	///////////////////////////////////////
	//_histDeviation = new Histogram("Deviation of projector pixels in camera space.", 256);
    
	ofAddListener(_histThresholdRange.updateSelection,this,&PCDecode::updateThresholdSelection);    
    
    _histThresholdRange.setSelection(_charCalibrateRangeThreshold);    
	///////////////////////////////////////

	
    
	///////////////////////////////////////
	// SCREENS
	///////////////////////////////////////
	_scrProjectorSpace = new scrTexture(_texCameraSpacePreview, "Projector space preview");
	ofAddListener(_scrProjectorSpace->evtCursorMove, this, &PCDecode::moveSendCursor);
	_scrCameraSpace = new scrTexture(_texProjectorSpacePreview, "Camera space preview");
	
    _scrCameraNFinds = new scrTexture(_texCameraSpaceNFinds, "NFinds");
	_scrProjectorNFinds = new scrTexture(_texProjectorSpaceNFinds, "NFinds");
    
    _scrHistograms = new scrHistograms("Histograms");
    _scrHistograms->addHistogram(_histThresholdRange);
    _scrHistograms->addHistogram(_histNFinds);
    
    _scrSubScans = &_subScanSet.screens;
	///////////////////////////////////////
    
    
    ///////////////////////////////////////
	// SUBSCANS
	///////////////////////////////////////
	//

    if (interleaveCount > 1)
    {
        // Interleaves
        
        SubScanInterleave *subScan;
        for (int i = 0; i < interleaveCount; i++)
        {
            subScan = new SubScanInterleave(_charCamera, _charCalibrateRangeThreshold, _boolProjectorMask, _histThresholdRange, i);
            _subScanSet.push(subScan);
        }
        
    } else {
        
        // Basic
        
        SubScanBasic *subScan;
        subScan = new SubScanBasic(_charCamera, _charCalibrateRangeThreshold, _boolProjectorMask, _histThresholdRange, 0);
        
        _subScanSet.push(subScan);
        
    }
	//
	///////////////////////////////////////	
}

PCDecode::~PCDecode()
{
	delete[] _charCameraSpacePreview;
	delete[] _charProjectorSpacePreview;
	
	delete _scrCameraSpace;
	delete _scrProjectorSpace;
	
}

bool PCDecode::capture(bool needFreshFrame)
{
	
	bool isFrameNew = _camera->capture(_charCamera, needFreshFrame);
	
	_texCamera.loadData(_charCamera, camWidth, camHeight, GL_LUMINANCE);
    
    _subScanSet.calcBinary();
	
	if (!isFrameNew && needFreshFrame)
		cout << "wait..";
    
	return isFrameNew;

}

void PCDecode::updateSpacePreview(int width, int height, unsigned char data[], ofTexture &texture, std::vector<PCPixel*> &foundPixels)
{
	float Xdash, Ydash;
	int nPixels = width * height;
	
	for (int iPixel=0; iPixel<nPixels; iPixel++)
	{
		Xdash = foundPixels[iPixel]->xdash.x;
		Ydash = foundPixels[iPixel]->xdash.y;
		
		data[iPixel*3] = float(255) * Xdash;
		data[iPixel*3+1] = float(255) * Ydash;
		data[iPixel*3+2] = 0;
	}
	texture.loadData(data, width, height, GL_RGB);	
}

void PCDecode::updateCameraSpacePreview()
{
	updateSpacePreview(projWidth,projHeight,_charCameraSpacePreview,_texCameraSpacePreview, projPixels);
	
	///////////////////////////
	// NFINDS
	///////////////////////////
	int maxNFinds=0;
	//find maximum
	for (int iPixel=0; iPixel<camPixelCount; iPixel++)
		if (camPixels[iPixel]->nFinds > maxNFinds)
			maxNFinds = camPixels[iPixel]->nFinds;
	//convert to float for division
	float fMaxNFinds = maxNFinds;
	//fill the char
	for (int iPixel=0; iPixel<camPixelCount; iPixel++)
		_charCameraSpaceNFinds[iPixel] = 255.0f *
				float(camPixels[iPixel]->nFinds) / fMaxNFinds;
	
	_texCameraSpaceNFinds.loadData(_charCameraSpaceNFinds, camWidth, camHeight, GL_LUMINANCE);
	///////////////////////////
			
}

void PCDecode::updateProjectorSpacePreview()
{
	updateSpacePreview(camWidth,camHeight,_charProjectorSpacePreview,_texProjectorSpacePreview, camPixels);
	
	///////////////////////////
	// NFINDS
	///////////////////////////
	int maxNFinds=0;
	//find maximum
	//start from 1 because 0 is where a lot of crap ends up
	for (int iPixel=1; iPixel<projPixelCount; iPixel++)
		if (projPixels[iPixel]->nFinds > maxNFinds)
			maxNFinds = projPixels[iPixel]->nFinds;
	ofLog(OF_LOG_VERBOSE, "PCDecode: max nFinds per projector pix is " + ofToString(maxNFinds,0));
	//convert to float for division
	float fMaxNFinds = maxNFinds;
	//fill the char
	for (int iPixel=0; iPixel<projPixelCount; iPixel++)
		_charProjectorSpaceNFinds[iPixel] = 255.0f *
		log(float(projPixels[iPixel]->nFinds)) / log(fMaxNFinds);
	
	_texProjectorSpaceNFinds.loadData(_charProjectorSpaceNFinds, projWidth, projHeight, GL_LUMINANCE);
	///////////////////////////
}

void PCDecode::moveSendCursor(ofPoint &ptCursorPosition)
{
	/*
	//this function will only work with sdev type PCPixels

	float &x = ptCursorPosition.x;
	float &y = ptCursorPosition.y;
	std::vector<ofPoint> *vecXdash;
	
	int iPixelX, iPixelY, iPixel;
	
	iPixelX = x*projWidth;
	iPixelY = y*projHeight;
	iPixel = iPixelY * projWidth + iPixelX;
	
	if (iPixel>0 && iPixel < projWidth * projHeight)
	{
		projPixels.at(iPixel)->getData(&vecXdash);
		
		_scrFrameData->setMarkers(vecXdash);
		
		_scrFrameData->updateInterface();
	} else {
		_scrFrameData->clearMarkers();
	}
	*/
}

void PCDecode::initCalibration()
{
    _subScanSet.initCalibration();
}

void PCDecode::addScanFrame(int iSubScanFrame, int iSubScan)
{
	
	if (iSubScanFrame==0)
		clearSubScan(iSubScan);
	    
    _subScanSet[iSubScan]->addScanFrame(iSubScanFrame);

    int &nScanFramesPerInterleave(Payload::Pointer->dataFramesPerInterleave);
    
	if(iSubScanFrame == nScanFramesPerInterleave-1)
        _subScanSet[iSubScan]->checkParity();

}

void PCDecode::addCalibrationFrame(int iSubScan)
{
    if (iSubScan == -1)
        _subScanSet.addCalibrationLow();
    else
        _subScanSet[iSubScan]->addCalibrationHigh();
}


void PCDecode::calcThreshold()
{
	
    _histThresholdRange.clear();
	
    _subScanSet.calcCalibration();
}

void PCDecode::calcInterleave(int iSubScan)
{
    
    _subScanSet[iSubScan]->calc(camPixels, projPixels);

	updateProjectorSpacePreview();
	updateCameraSpacePreview();
	
	_camera->clearTimer();
}

void PCDecode::clearSubScan(int iSubScan)
{
    _subScanSet[iSubScan]->clearData();
}

void PCDecode::clear(bool clearFinds)
{
	if (clearFinds)
	{

		for (int iProjectorPixel = 0; iProjectorPixel<projPixelCount; iProjectorPixel++)
			projPixels.at(iProjectorPixel)->clear();
		
		for (int iCameraPixel = 0; iCameraPixel < camPixelCount; iCameraPixel++)
			camPixels.at(iCameraPixel)->clear();
	}
    
    _subScanSet.clearData();

}

void PCDecode::updateThresholdSelection(int &iSelectionClass)
{
    _subScanSet.updateThresholdSelection(iSelectionClass);
}

int PCDecode::getNFinds(int iProjectorPixel)
{
	if (iProjectorPixel>=0 && iProjectorPixel<projPixelCount)
		return projPixels.at(iProjectorPixel)->nFinds;
	else
		return 0;
}


void PCDecode::getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, int &iLastFoundCameraPixel)
{
	
	if (iProjectorPixel>=0 && iProjectorPixel<projPixelCount)
	{
		CamMeanX = projPixels[iProjectorPixel]->xdash.x;
		CamMeanY = projPixels[iProjectorPixel]->xdash.y;
		iLastFoundCameraPixel = projPixels[iProjectorPixel]->iLastFoundPixel;
	} else
		ofLog(OF_LOG_ERROR, "PCEncode::getFoundPixelData : Attempted to access Pixel ID " + ofToString(iProjectorPixel) + " which is out of range");
}

/*
only available if sdev exists
void PCDecode::getFoundPixelData(int iProjectorPixel, float &CamMeanX, float &CamMeanY, float &CamSigmaX, float &CamSigmaY, int &iLastFoundCameraPixel)
{
	
	if (iProjectorPixel>=0 && iProjectorPixel<projPixelCount)
		projPixels.at(iProjectorPixel)->getData(CamMeanX, CamMeanY, CamSigmaX, CamSigmaY, iLastFoundCameraPixel);	
	else
		ofLog(OF_LOG_ERROR, "PCEncode::getFoundPixelData : Attempted to access Pixel ID " + ofToString(iProjectorPixel) + " which is out of range");
}

*/