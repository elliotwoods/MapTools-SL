//
//  CameraEdsdk.cpp
//  MapToolsSL
//
//  Created by Elliot Woods on 05/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CameraEdsdk.h"

CameraEdsdk::CameraEdsdk() :
waitingForFrame(false) {
	fnum = 0;
}

void CameraEdsdk::close() {
	cam.close();
}

bool CameraEdsdk::isFrameNew() {
	if (cam.isPhotoNew() && waitingForFrame) {
		copyToGrey();
		waitingForFrame = false;
		return true;
	} else {
		return false;
	}
}

bool CameraEdsdk::init(int ID, int width, int height) {
	bool success = cam.setup(ID);
	preview = &cam.getLiveTextureReference();
	return success;
}

string CameraEdsdk::getImageName() {
	return "edsdkframe" + ofToString(_ID) + ".jpg";
}

void CameraEdsdk::grab() {
	if (!waitingForFrame) {
		cam.takePhoto(false);
		waitingForFrame = true;
	}
}

void CameraEdsdk::grabPreview() {
	cam.update();
}

void CameraEdsdk::copyToGrey() {
	Mat rgb = toCv(cam.getPhotoPixels());
	Mat grey, grey2; // native resolution
	
	cvtColor(rgb, grey, CV_RGB2GRAY);
	resize(grey, grey2, cv::Size(greyPixels.getWidth(), greyPixels.getHeight()), 0, 0, INTER_CUBIC);
	copy(grey2, greyPixels);
	
	ofSaveImage(greyPixels, "cap" + ofToString(fnum++) + ".jpg");
}