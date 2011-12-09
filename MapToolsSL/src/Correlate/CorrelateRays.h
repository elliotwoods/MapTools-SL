#pragma once
//
//  CorrelateRays.h
//  MapTools-SL
//
//  Created by Elliot Woods on 04/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxCv/Calibration.h"

#include "Ray.h"

#define CV_SCALE_FACTOR 100.0f

using namespace ofxCv;

typedef struct {
	ofVec2f xy[2];
} CamPoint;


class CorrelateRays {
public:
	CorrelateRays();
	
	void	load();
	
	void	evaluate(const vector<CamPoint> &c, vector<ofVec3f> &w, float threshold=0.05); 
	
	void	getRay(const CamPoint &c, int iCamera, Ray& r);
	
	Calibration		calibration[2];
	ofMatrix4x4		matPosRotFromXtoOther[2];
	ofMatrix4x4		matP[2], matPInverse[2];
	bool			loaded;	
	
protected:
	void	calcMatrices();
	
	float	width, height;
	
	float	focalX[2], focalY[2];
	float	principalX[2], principalY[2];
};