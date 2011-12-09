//
//  scrSelectBounds.cpp
//  MapTools Structured Light
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "scrSelectBounds.h"

scrSelectBounds::scrSelectBounds(string caption) :
scrPointCloud(caption),
lbf(-0.8,-0.6,0.9),
rtb(0.4,0.5,3.0)
{
    shader.load("selectBounds");
    shader.printActiveUniforms();
}

void scrSelectBounds::drawContent()
{
    //setup view
    begin();
    
    ofPushStyle();
    ofSetColor(255.0f, 255.0f, 255.0f);
    ofSetLineWidth(2);
    
    glBegin(GL_LINE_STRIP);
    
    //bottom
    glVertex3f(lbf.x, lbf.y, lbf.z);
    glVertex3f(rtb.x, lbf.y, lbf.z);
    glVertex3f(rtb.x, lbf.y, rtb.z);
    glVertex3f(lbf.x, lbf.y, rtb.z);
    glVertex3f(lbf.x, lbf.y, lbf.z);
    
    //top
    glVertex3f(lbf.x, rtb.y, lbf.z);
    glVertex3f(rtb.x, rtb.y, lbf.z);
    glVertex3f(rtb.x, rtb.y, rtb.z);
    glVertex3f(lbf.x, rtb.y, rtb.z);
    glVertex3f(lbf.x, rtb.y, lbf.z);
    
    glEnd();
    
    //last 3 lines
    glBegin(GL_LINES);
    
    glVertex3f(rtb.x, lbf.y, lbf.z);
    glVertex3f(rtb.x, rtb.y, lbf.z);
    
    glVertex3f(lbf.x, lbf.y, rtb.z);
    glVertex3f(lbf.x, rtb.y, rtb.z);
    
    glVertex3f(rtb.x, lbf.y, rtb.z);
    glVertex3f(rtb.x, rtb.y, rtb.z);
    
    glEnd();
    
    
    ofPopStyle();
    
    
    

    ////bind shader
    //shader.begin();

    ////set properties
    //shader.setUniform1f("testFloat", (float)sin(ofGetElapsedTimef())/2.0f + 0.5f);
    //shader.setUniform3f("lbf", lbf.x, lbf.y, lbf.z);
    //shader.setUniform3f("rtb", rtb.x, rtb.y, rtb.z);
    //
    
    //draw content
    drawPoints();
    
    //unbind shader
//	shader.end();
    
    //restore view
    end();
}