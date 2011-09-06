/*
 *  PCLogger.cpp
 *  pixel correlation
 *
 *  Created by Elliot Woods on 26/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCLogger.h"

PCLogger::PCLogger(PCEncode *encoder, vector<PCDecode*> *decoders)
{
	_encoder = encoder;
	_decoders = decoders;
	
	_imgCameraSpacePreview.setUseTexture(false);
	_imgProjectorSpacePreview.setUseTexture(false);
	
}

void PCLogger::save(string filename)
{
	// ** TODO ** 
	//this needs to be replaced with a kind
	//'what to log' rather than
	//'whether to log'
	if (!isLogging)
		return;
	
	#ifdef TARGET_WIN32
		filename = "logs\\" + filename;
	#else
		filename = "logs/" + filename;
	#endif

	//////////////////////////////////////////////////////
	// SAVE IMAGES
	//////////////////////////////////////////////////////
	for (int iDecoder=0; iDecoder<_decoders->size(); iDecoder++)
	{
		
		string strExtension = ".png";
		string strIndex = "[" + ofToString(iDecoder,0) + "]";
		
		
		_imgCameraSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charCameraSpacePreview,
											 projWidth, projHeight,
											 OF_IMAGE_COLOR, true);
		
		_imgProjectorSpacePreview.setFromPixels(_decoders->at(iDecoder)->_charProjectorSpacePreview,
												camWidth, camHeight,
												OF_IMAGE_COLOR, true);
		
		_imgCameraSpacePreview.saveImage(filename + "-camera" + strIndex + strExtension);
		_imgProjectorSpacePreview.saveImage(filename + "-projector" + strIndex + strExtension);
	}
	//////////////////////////////////////////////////////

	//////////////////////////////////////////////////////
	// SAVE DATA
	//////////////////////////////////////////////////////
	//first we have to create the filename
	//with path for iostream to access file
	string strFileWithPath;
#ifdef TARGET_WIN32
	strFileWithPath = ".\\data\\" + filename;
#else
	strFileWithPath = "../../../data/" + filename;
#endif
	
	
	saveConfig(strFileWithPath + "-config.txt");
	savePixelsText(strFileWithPath + "-data.txt");
	savePixelsBinary(strFileWithPath + ".bin");
	//////////////////////////////////////////////////////

}

void PCLogger::saveConfig(string filename)
{
	ofstream iofOutput(filename.c_str(), ios::out);
	iofOutput	<< projWidth << "\t"
				<< projHeight << "\t"
				<< interleaveWidth << "\t"
				<< camWidth << "\t"
				<< camHeight << "\t"
				<< _decoders->size() << endl;
	
	iofOutput.close();
}

void PCLogger::savePixelsBinary(string filename)
{
	ofstream iofOutput(filename.c_str(), ios::out | ios::binary);
    
	bool hasAllFinds;
	PCPixel *currentPixel;
	
	unsigned short iPX, iPY;
	float fPX, fPY;
	
	//write config
	iofOutput.write((char*) &nCameras, 1);
	iofOutput.write((char*) &projWidth, 2);
	iofOutput.write((char*) &projHeight, 2);
	//could write rest of config here if it's useful..
	
	for (int iPP=0; iPP<projPixelCount; iPP++)
	{
		//////////////////////////////
		//check exists in all cameras
		//////////////////////////////
		hasAllFinds = true;
		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			currentPixel = _decoders->at(iDec)->projPixels[iPP];
			hasAllFinds &= currentPixel->iLastFoundPixel != -1;
		}
		if (!hasAllFinds)
			continue;
		//////////////////////////////
		
		
		//////////////////////////////
		//write projector data
		//////////////////////////////
		iPX = iPP % projWidth;
		iPY = iPP / projWidth;
		
		fPX = float(iPX) / float (projWidth);
		fPY = float(iPY) / float (projHeight);
		
		//write the projector pixels' ID
		iofOutput.write((char*) &iPX, 2);
		iofOutput.write((char*) &iPY, 2);
		//write the projector pixels' normalised position
		iofOutput.write((char*) &fPX, 4);
		iofOutput.write((char*) &fPY, 4);
		//////////////////////////////
								
		
		//////////////////////////////
		//write camera data
		//////////////////////////////
		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			currentPixel = _decoders->at(iDec)->projPixels[iPP];
			iofOutput.write((char*) &(currentPixel->xdash.x), 4);
			iofOutput.write((char*) &(currentPixel->xdash.y), 4);
		}
		//////////////////////////////

    }
	
    iofOutput.close();
}

void PCLogger::savePixelsText(string filename)
{
	//save in row format:
	
	//	i	ix	iy
	//				meanX	meanY	sigmaX	sigmaY sigmaR	iLastFind	nFinds
	
	ofstream iofOutput(filename.c_str(), ios::out);
	stringstream dataRow;
	
	dataRow.precision(10);
	
	PCPixel *pixelFinds;
	
	bool hasAllFinds;

	for (int iPP=0; iPP<projPixelCount; iPP++)
	{
		dataRow.str("");

		dataRow << iPP << "\t" <<
			(iPP % projWidth) << "\t" <<
			int(iPP / projWidth);

		hasAllFinds = true;

		for (int iDec=0; iDec<_decoders->size(); iDec++)
		{
			pixelFinds = _decoders->at(iDec)->projPixels[iPP];
	
			dataRow << "\t"<< pixelFinds->xdash.x << "\t" << pixelFinds->xdash.y << "\t" <<
				//only for sdev
				/*
								pixelFinds->sigmaXdash.x << "\t" <<  pixelFinds->sigmaXdash.y << "\t" <<
								pixelFinds->sigmaRdash << "\t" << 
				*/
								pixelFinds->iLastFoundPixel << "\t" << 
								pixelFinds->nFinds;

			hasAllFinds &= (pixelFinds->iLastFoundPixel != -1);
		}
			
		dataRow << endl;

		if (hasAllFinds)
			iofOutput << dataRow.str();
	}

    iofOutput.close();
}