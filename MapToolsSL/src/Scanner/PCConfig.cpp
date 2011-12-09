/*
 *  PCConfig.cpp
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCConfig.h"

bool PCConfig::sdev = false;
bool PCConfig::isLogging=true;

float PCConfig::thresholdPercentile=0.5;
int PCConfig::captureDelay=200;
float PCConfig::maxPixelMoveSquared = 0.01;

int PCConfig::gain=CONFIG_DEFAULT_GAIN;
int PCConfig::exposure=CONFIG_DEFAULT_EXPOSURE;

char PCConfig::nCameras=0;
vector<int> PCConfig::camIDs = vector<int>();

unsigned short PCConfig::camWidth=0;
unsigned short PCConfig::camHeight=0;
int PCConfig::camPixelCount=0;

unsigned short PCConfig::projWidth=0;
unsigned short PCConfig::projHeight=0;
int PCConfig::projPixelCount=0;

unsigned short PCConfig::interleaveWidth=0;
unsigned short PCConfig::interleaveHeight=0;
unsigned short PCConfig::interleaveCount=0;

unsigned short PCConfig::errorBits=0;

string PCConfig::_configFilename="";
bool PCConfig::_loaded=false;

PCConfig::PCConfig()
{
	if (_loaded)
		return;

	_configFilename = "settings.xml";

    //setup some default vars.
    //
    //does this make any sesnse?
    //we're already doing this in xml load
    //
	captureDelay = 100; //ms
	thresholdPercentile = 0.5;
	
	_loaded = configLoad();

	if (!_loaded)
	{
		cout << "load settings failed";
		std::exit(1);
	}
	
}

bool PCConfig::configLoad()
{
	return configLoad(_configFilename);
}

bool PCConfig::configLoad(string filename)
{
	bool success;
	ofxXmlSettings configFileXML;
	configFileXML.clear();
	
	success = configFileXML.loadFile(filename);
//	if (!success)
//		/** HACK **/
//#ifdef TARGET_OSX
//		success = configFileXML.loadFile("/Users/elliot/dev/openFrameworks/openFrameworks/apps/MapTools-SL/MapToolsSL/bin/data/" + filename);
//	#else
//		success = configFileXML.loadFile("c:\\openFrameworks\\007\\apps\\MapTools-SL\\MapToolsSL\\bin\\settings.xml");
//#endif
	if (success)
	{
		try
		{
			if (!configFileXML.pushTag("settings", 0))
				throw;
			
			if (configFileXML.pushTag("encoding", 0))
			{
				projWidth = configFileXML.getAttribute("projector", "width", 256, 0);
				projHeight = configFileXML.getAttribute("projector", "height", 256, 0);

				interleaveWidth = configFileXML.getAttribute("interleave", "width", 1, 0);
				interleaveHeight = configFileXML.getAttribute("interleave", "height", 1, 0);
				
				errorBits = configFileXML.getAttribute("errorcheck", "nFrames", 4, 0);
				
				configFileXML.popTag();
			} else
				throw;
			
			if (configFileXML.pushTag("decoding", 0))
			{
				camWidth = configFileXML.getAttribute("cameras", "width", 640, 0);
				camHeight = configFileXML.getAttribute("cameras", "height", 480, 0);
				captureDelay = configFileXML.getAttribute("cameras", "delay", 100, 0);
				exposure = configFileXML.getAttribute("cameras", "exposure", CONFIG_DEFAULT_EXPOSURE, 0);
				gain = configFileXML.getAttribute("cameras", "gain", CONFIG_DEFAULT_GAIN, 0);
				
				if (configFileXML.pushTag("cameras", 0))
				{
					nCameras = configFileXML.getNumTags("camera");
					
					//read camera IDs into vector
					camIDs.clear();
					for (int iCam=0; iCam<nCameras; iCam++)
						camIDs.push_back(configFileXML.getAttribute("camera", "id", 0, iCam));
					
					configFileXML.popTag();
				} else
					throw;
				
				thresholdPercentile = configFileXML.getAttribute("threshold", "percentile", 0.5, 0);
                
				sdev = configFileXML.getAttribute("data", "sdev", 0, 0);
				
                maxPixelMoveSquared = configFileXML.getAttribute("PCPixel", "maxPixelMoveSquared", 0.01, 0);
                
				configFileXML.popTag();
			} else
				throw;
			
			isLogging = configFileXML.getAttribute("logging", "on", 0, 0) > 0;
			
		}
		catch(...)
		{
			success=false;
		}
	}

	
	///**HACK**/
	//if (!success) {
	//	projWidth = 1024;
	//	projHeight = 768;

	//	interleaveWidth = 2;
	//	interleaveHeight = 2;
	//	errorBits = 0;

	//	camWidth = 1280;
	//	camHeight = 720;
	//	captureDelay = 1000;
	//	camIDs.push_back(0);
	//	camIDs.push_back(1);
	//	nCameras = camIDs.size();

	//	thresholdPercentile = 0.7;
	//	sdev = 0;
	//	maxPixelMoveSquared = 100;
	//	isLogging = true;
	//	success = true;
	//}

	if (!success)
		cout << "Loading settings file " << _configFilename << " failed.\n";
	else
		cout << "Loading settings file " << _configFilename << " success.\n";
    
    calc();
	
	return success;
}

void PCConfig::calc()
{
    camPixelCount = camWidth * camHeight;
    projPixelCount = projWidth * projHeight;
    interleaveCount = interleaveWidth * interleaveHeight;
}