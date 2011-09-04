//
//  SecondCamera.cpp
//  intersectRayTest2
//
//  Created by Elliot Woods on 02/09/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "SecondCamera.h"

void SecondCamera::begin(ofRectangle viewport)
{
	ofPushView();
	ofViewport(viewport);
	
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(matProjection.getPtr());
	
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(matModelView.getPtr());
}

void SecondCamera::setOffset(ofVec3f offset)
{
	_offset = offset;
}

void SecondCamera::setFromOtherCamera(ofCamera& other)
{
	setFarClip(other.getFarClip());
	setNearClip(other.getNearClip());
	setFov(other.getFov());
	
	matProjection = other.getProjectionMatrix();
	matModelView = other.getModelViewMatrix();
	
	matModelView.postMultTranslate(-_offset);
	
	isActive = true;
}