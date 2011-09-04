#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(80,80,80);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	glEnable(GL_DEPTH_TEST);
	
	cam1.setDistance(5.0f);
	cam1.setDrag(0.01);
	cam1.setFarClip(1000.0f);
	cam1.setNearClip(0.1f);
	
	intersecter1.color = ofColor(255,255,0);
	intersecter2.color = ofColor(0,255,255);
	
	randomiseRays();
	setupViewports();
	
	cam1.cacheMatrices(true);
	cam2.cacheMatrices(true);
	isMoving = false;
}

void testApp::setupViewports(){
	
	viewports[1].width = viewports[0].width = ofGetWidth()/2.0f;
	
	viewports[1].height = viewports[0].height = ofGetHeight();
	
	viewports[0].x = 0;
	viewports[1].x = viewports[0].width;
	viewports[0].y = viewports[1].y = 0;
	
	cam2.setOffset(ofVec3f(0.5, 0, 0));
}

//--------------------------------------------------------------
void testApp::update(){
	cam2.setFromOtherCamera(cam1);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofPushStyle();
	ofSetLineWidth(1);
	ofNoFill();
	ofRect(viewports[0]);
	ofRect(viewports[1]);
	ofPopStyle();
	
	
	ofPushStyle();
	ofEnableSmoothing();
	
	cam1.begin(viewports[0]);
	drawScene();
	cam1.end();
	
	cam2.begin(viewports[1]);
	drawScene();
	cam2.end();
	
	ofPopStyle();
}

void testApp::drawScene(){
	ofPushStyle();
	ofEnableSmoothing();
	ofDrawAxis(1.0f);
	ofSetColor(120,120,120);
	ofDrawGrid(5.0f);
	ofPopStyle();
	
	ray1.draw();
	ray2.draw();
	intersecter1.draw(5.0f);
	intersecter2.draw(5.0f);
}

void testApp::randomiseRays() {
	ray1.randomise();
	ray2.randomise();
	
	updateIntersects();
}

void testApp::updateIntersects() {
	intersecter1 = ray1.intersect(ray2);
	intersecter2 = ray2.intersect(ray1);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key=='r')
		randomiseRays();
	
	if (key==' ')
	{
		isMoving ^= true;
		
		float bgcol = isMoving ? 140 : 80;
		ofBackground(bgcol, bgcol, bgcol);
		
		cam1.setDrag(isMoving ? 0 : 0.01);
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	if (!isMoving)
		return;
	
	ofVec3f neg;
	ofVec3f pos;
	
	bool secondport = (x > viewports[0].width);
		
	x = secondport ? x - viewports[0].width : x;
	
	neg.x = float(x) / viewports[0].width * 2 - 1;
	neg.y = 1 - float(y) / viewports[0].height * 2;
	neg.z = -1;
	
	pos.x = neg.x;
	pos.y = neg.y;
	pos.z = 1;
	
	Ray *r;
	ofCamera* c;
	if (!secondport)
	{
		r = &ray1;
		c = &cam1;
	} else {
		r = &ray2;
		c = &cam2;
	}
	
	ofMatrix4x4 iMVP = c->getModelViewProjectionMatrix().getInverse();
	r->s = neg * iMVP;
	r->t = pos * iMVP - r->s;
	
	updateIntersects();
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	if (isMoving)
		mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	setupViewports();
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}