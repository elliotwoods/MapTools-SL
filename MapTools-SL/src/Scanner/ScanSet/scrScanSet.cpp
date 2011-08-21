//
//  scrScanSet.cpp
//  PC Encode
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "scrScanSet.h"

ofPoint scrScanSet::spin = ofPoint();
ofPoint scrScanSet::translate = ofPoint();
float scrScanSet::distance = 1;


scrScanSet::scrScanSet(string caption, ScanSet &set) :
scrBase(caption),
_set(set)
{
	pointSize = 2;
	glGenBuffersARB(1, &vbo[0]);
	glGenBuffersARB(1, &vbo[1]);
}

scrScanSet::~scrScanSet()
{

}

void scrScanSet::drawContent()
{
    if (_set.nPoints == 0)
        return;
    
    begin();
    drawPoints();
    end();
	
}

void scrScanSet::begin()
{
    ofPushStyle();
	
	//temporarily store the viewport
	glGetIntegerv(GL_VIEWPORT, _viewport_temp);
	
	//set the viewport to our screen only
	int boundsx,boundsy,boundswidth,boundsheight;
	getLiveBounds(boundsx, boundsy, boundswidth, boundsheight);
	boundsy = ofGetHeight()-boundsy-boundsheight;
	glViewport(boundsx, boundsy, boundswidth, boundsheight);
	
	//camera
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(50.0f, float(boundswidth)/float(boundsheight), 0.1f, 10.0f);
	gluLookAt(0, 0, -distance, 0, 0, 0, 0, 1, 0);
	
	//modelview
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(spin.x, 1, 0, 0);
	glRotatef(spin.y, 0, 1, 0);
	glTranslatef(translate.x, translate.y, translate.z);
	
	glEnable(GL_DEPTH_TEST);
	glPointSize(pointSize);
}

void scrScanSet::drawPoints()
{
    glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[0]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, _set.nPoints * 3 * sizeof(float), _set.xyz, GL_STATIC_DRAW_ARB);
	glVertexPointer(3, GL_FLOAT, 0, 0);
    
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, vbo[1]);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB, _set.nPoints * 3 * sizeof(float), _set.rgb, GL_STATIC_DRAW_ARB);
	glColorPointer(3, GL_FLOAT, 0, 0);
	
	glDrawArrays(GL_POINTS, 0, _set.nPoints);
	
	glDisableClientState(GL_VERTEX_ARRAY); 
	glDisableClientState(GL_COLOR_ARRAY);
	
	glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
}

void scrScanSet::end()
{
    //clear world states
	glDisable(GL_DEPTH_TEST);	
	
	//clear matricies
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	//let's get our full viewport back
	glViewport(_viewport_temp[0], _viewport_temp[1],
			   _viewport_temp[2], _viewport_temp[3]);
    
    int x, y, w, h;
    getLiveBounds(x,y,w,h);
    
    ofDrawBitmapString(ofToString(_set.nPoints), x + 10, y + 30);
}

void scrScanSet::mouseDragged(int x, int y, int dx, int dy, int button)
{
	int boundsx, boundsy, boundswidth, boundsheight;
	getLiveBounds(boundsx, boundsy, boundswidth, boundsheight);
	
	if (button==0)
	{
		spin.x += float(dy) / float(boundsheight) * 360.0f;
		spin.y -= float(dx) / float(boundswidth) * 360.0f;
		
		spin.x = ofClamp(spin.x, -180, 180);
	} else {
		distance +=float(dy) / float(boundsheight) * 4.0f;
		distance = ofClamp(distance, 0.1, 10);
		
		translate.z +=float(dx) / float(boundswidth) * 2.0f;
	}
}
