#pragma once
//
//  CameraEdsdk.h
//  MapToolsSL
//
//  Created by Elliot Woods on 05/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxEdsdk.h"
#include "ofxCv.h"

#include "CameraBase.h"

using namespace ofxCv;
using namespace ofxEdsdk;

class CameraEdsdk : public CameraBase {
public:
	CameraEdsdk();
	
    void	close();
    bool	isFrameNew();
	
protected:
    bool	init(int ID, int width, int height);
    void	grab();
	void	grabPreview();
	
	void	copyToGrey();
	string getImageName();
	ofImage imgLoader;
	Camera cam;
	
	bool waitingForFrame;
	int fnum;
};