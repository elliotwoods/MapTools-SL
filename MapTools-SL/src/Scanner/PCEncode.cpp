/*
 *  PCEncode.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 25/10/2009.
 *  Copyright 2009 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCEncode.h"

PCEncode::PCEncode(bool* boolProjectorMask) : 
scrSend(_texOutput, "Message")
{	
	_texCharOutput = new unsigned char[projPixelCount];
	_texOutput.allocate(projWidth,projHeight,GL_LUMINANCE);
	_texOutput.setTextureMinMagFilter(GL_NEAREST, GL_NEAREST);
	
	_boolProjectorMask = boolProjectorMask;
}

PCEncode::~PCEncode()
{
	delete _texCharOutput;
}

void PCEncode::updateScanFrame(int iScanInterleaveFrame, int iInterleave)
{
	int iPixel, iInterleavePixel;
	int iInterleaveX, iInterleaveY;
	int binaryFrame, binaryParityFrame;
	int iScanParityFrame;
	bool isOn;
	
	iInterleaveX = iInterleave % interleaveWidth;
	iInterleaveY = iInterleave / interleaveHeight;

	iScanParityFrame = iScanInterleaveFrame - Payload::Pointer->dataFramesPerInterleave;
	
	binaryFrame = (1<<iScanInterleaveFrame);
	binaryParityFrame = (1<<iScanParityFrame);
	
	//reset all the pixels to black
	memset(_texCharOutput, 0, projPixelCount);

	
	//**TODO** calculations here presume
	// projWidth % interleaveWidth = 0
	// projHeight % interleaveHeight = 0
	// and in lots of other places
	// maybe we should check that on load?

	//loop through the pixels for this interleave
	for (int iIX = 0; iIX < projWidth/interleaveWidth; iIX++)

		for (int iIY = 0; iIY < projHeight/interleaveHeight; iIY++)
		{
			iPixel = (iIX*interleaveWidth + iInterleaveX)
				+ (iIY*interleaveHeight + iInterleaveY) * projWidth;
			
			// should be ceil(double(_projWidth)/double(_interleaveWidth))
			// but that may be slow??(probably not)
			iInterleavePixel = iIX +
								iIY* (projWidth/interleaveWidth);
			
			if (iScanInterleaveFrame < Payload::Pointer->dataFramesPerInterleave)
				//send index data
				isOn=(Payload::Pointer->data[iInterleavePixel] & binaryFrame)>>iScanInterleaveFrame;
			else
				//send error check
				isOn=(Payload::Pointer->errorCheck[iInterleavePixel] & binaryParityFrame)>>iScanParityFrame;
			
			_texCharOutput[iPixel]= 255 * isOn * _boolProjectorMask[iPixel];
		}

	
	_texOutput.loadData(_texCharOutput, projWidth,projHeight, GL_LUMINANCE);
}

void PCEncode::updateCalibrationFrame(int iCalibrationFrame)
{
	if (iCalibrationFrame==0)
		ofLog(OF_LOG_VERBOSE, "calib frame 0");
	
	int iPixel, iX, iY;
	int iInterleave, iInterleaveX, iInterleaveY;
	
	iInterleave = iCalibrationFrame-1;
	iInterleaveX = iInterleave % interleaveWidth;
	iInterleaveY = iInterleave / interleaveHeight;
	
	for (iX = 0; iX < projWidth; iX++)
		for (iY = 0; iY < projHeight; iY++)
		{
			iPixel=iY*projWidth + iX;
			
			if (iCalibrationFrame==0)
			{
				_texCharOutput[iPixel]=0;
			} else {
				
				if ((iX % interleaveWidth == iInterleaveX) &&
					(iY % interleaveHeight == iInterleaveY))
					_texCharOutput[iPixel]=255  * _boolProjectorMask[iPixel];
				else
					_texCharOutput[iPixel]=0;

			}
		}
	
	_texOutput.loadData(_texCharOutput, projWidth,projHeight, GL_LUMINANCE);
}
