/*
 *  PCLogger.h
 *  pixel correlation
 *
 *  Created by Elliot Woods on 26/01/2010.
 *  Copyright 2010 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCconstants.h"
#include "PCConfig.h"
#include "PCEncode.h"
#include "PCDecode.h"
#include "ofMain.h"

#include <ctime>
#include <iostream>
#include <fstream>

class PCLogger : public PCConfig
{
	
public:
	PCLogger(PCEncode *encoder, vector<PCDecode*> *decoders);
	
	void		save(string filename);
	
private:
	void		saveConfig(string filename);
	void		savePixelsBinary(string filename);
	void		savePixelsText(string filename);
	
	vector<PCDecode*>	*_decoders;
	PCEncode			*_encoder;	
	
	ofImage		_imgCameraSpacePreview, _imgProjectorSpacePreview;

};