#pragma once
/*
 *  PCConfig.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#define CONFIG_DEFAULT_EXPOSURE 250
#define CONFIG_DEFAULT_GAIN 30


#include "PCconstants.h"
#include "ofxXmlSettings.h"

class PCConfig
{
	
	public:
		PCConfig();
	
		bool	configLoad();
		bool	configLoad(string filename);
		bool	configSave();
		bool	configSave(string filename);	

		static char		nCameras;
	
		static vector<int> camIDs;
        
		static unsigned short		camWidth, camHeight;
        static int                  camPixelCount;
        static unsigned short		projWidth, projHeight;
        static int                  projPixelCount;
		static unsigned short		interleaveWidth, interleaveHeight;
        static unsigned short       interleaveCount;
		static unsigned short		errorBits;
		
		static int		exposure, gain;
		static int		captureDelay;
		static float	thresholdPercentile;
        static float    maxPixelMoveSquared;
    
		static bool		sdev;
		static bool		isLogging;
	
	private:
        void            calc();
		static string	_configFilename;
		static bool		_loaded;

		double Log2(double n) { return log( n ) / log((double)2); };

};