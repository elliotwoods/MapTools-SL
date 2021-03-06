//
//  CorrelateRays.cpp
//  MapTools-SL
//
//  Created by Elliot Woods on 04/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CorrelateRays.h"

CorrelateRays::CorrelateRays() {
	loaded = false;
}

string getAttribName(int iCam, int i, int j) {
	return "Camera" + ofToString(iCam) + "Transform_" + ofToString(i) + "-" + ofToString(j);
}

void CorrelateRays::load() {
	FileStorage fs(ofToDataPath("transforms.xml", true), FileStorage::READ);
	
	for (int iCam=0; iCam<2; iCam++)
	{
		for (int i=0; i<4; ++i)
			for (int j=0; j<4; ++j)
				fs[getAttribName(iCam,i,j)] >> matPosRotFromXtoOther[iCam](i, j);
		
		calibration[iCam].load("calib-" + ofToString(iCam) + ".yml", true);
		
		Mat cam = calibration[iCam].getUndistortedIntrinsics().getCameraMatrix();
		focalX[iCam] = cam.at<double>(0,0);
		focalY[iCam] = cam.at<double>(1,1);
		principalX[iCam] = cam.at<double>(1,2);
		principalY[iCam] = cam.at<double>(1,2);
	}	
	
	cv::Size s = calibration[0].getUndistortedIntrinsics().getImageSize();
	width = s.width;
	height = s.height;

	
	calcMatrices();
	loaded = true;
}
									   
void CorrelateRays::calcMatrices() {

	for (int iCam=0; iCam<2; iCam++)
	{
		Mat matCv = calibration[iCam].getUndistortedIntrinsics().getCameraMatrix();
		ofMatrix4x4 matC;
		
		//////////
		//forwards
		//////////
		//
		matC.makeIdentityMatrix();
		for (int i=0; i<3; ++i)
			for (int j=0; j<3; ++j)
				matC(i<2?i:3,j<2?j:3) = matCv.at<double>(j, i);
		
		matP[iCam].makeIdentityMatrix();
		
		matP[iCam](3,3) = 0; //confirmed
		matP[iCam](2,3) = 1;
		
		matP[iCam] = matP[iCam] * matC; //confirmed
		//
		//////////
		
		
		//////////
		//inverse
		//////////
		//
		Mat matCvInv = matCv.inv(DECOMP_SVD);
		matC.makeIdentityMatrix();
		for (int i=0; i<3; ++i)
			for (int j=0; j<3; ++j)
				matC(i<2?i:3,j<2?j:3) = matCvInv.at<double>(j, i);
		matP[iCam].makeIdentityMatrix();
		matP[iCam](3,3) = 0; //confirmed
		matP[iCam](2,3) = 1;
		
		matPInverse[iCam] = matC * matP[iCam].getInverse();
		//
		///////////
		
	}
}

void CorrelateRays::evaluate(const vector<CamPoint> &c, ScanSet &s, float threshold) {
	int nPoints = c.size();
	
	Ray r1, r2, intersect;
	ofVec3f midpoint;
	ofVec3f *w = s.xyz;

	ofVec3f mean;
	int usedPoints = 0;

	for (int i=0; i<nPoints; ++i, w++) {
		getRay(c[i], 0, r1);
		getRay(c[i], 1, r2);
		intersect = r1.intersect(r2);

		if (intersect.getLength() > threshold * CV_SCALE_FACTOR)
			*w = ofVec3f(0);
		else {
			*w = intersect.getMidpoint() / CV_SCALE_FACTOR;
			mean += *w;
			usedPoints++;
		}
	}

	mean /= float(usedPoints);

	////
	//sdev
	////
	//
	ofVec3f deviation = ofVec3f(0);
	s.active = usedPoints;
	w = s.xyz;
	for (int i=0; i<usedPoints; i++, w++)
		if (w->length() > 0)
			deviation += (*w - mean) * (*w - mean);

	deviation /= (float)usedPoints;
	deviation.x = sqrt((double)deviation.x);
	deviation.y = sqrt((double)deviation.y);
	deviation.z = sqrt((double)deviation.z);
	//
	////

	s.lbf = mean - deviation;
	s.rtb = mean + deviation;
}

void CorrelateRays::getRay(const CamPoint &c, int iCamera, Ray &r) {
	ofVec3f dx;
	
	ofVec2f xyRaw;
	xyRaw.x = c.xy[iCamera].x * width;
	xyRaw.y = c.xy[iCamera].y * height;

	ofVec2f xy;
	xy = calibration[iCamera].undistort(xyRaw);
	
	xy.x = xy.x * focalX[iCamera] + principalX[iCamera];
	xy.y = xy.y * focalY[iCamera] + principalY[iCamera];  
	
	dx.x = xy.x;
	dx.y = xy.y;
	dx.z = 100.0f;
	
	r.s = ofVec3f(0,0,0);
	r.t = dx * matPInverse[iCamera];
	
	if (iCamera==1)
		r.transform(matPosRotFromXtoOther[1]);
}