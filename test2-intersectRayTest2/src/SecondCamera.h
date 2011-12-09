#pragma once
//
//  SecondCamera.h
//  intersectRayTest2
//
//  Created by Elliot Woods on 02/09/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"

class SecondCamera : public ofCamera
{
public:
	void begin(ofRectangle viewport = ofGetCurrentViewport());

	void setOffset(ofVec3f offset);
	void setFromOtherCamera(ofCamera& other);
	
protected:
	ofVec3f _offset;
};