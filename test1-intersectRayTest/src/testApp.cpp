#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(80,80,80);
	ofSetVerticalSync(true);
	
	cam.setDistance(5.0f);
	cam.setDrag(0.01);
	cam.setFarClip(1000.0f);
	
	intersecter1.color = ofColor(255,255,0);
	intersecter2.color = ofColor(0,255,255);
	
	randomiseRays();
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofPushStyle();
	ofEnableSmoothing();
	
	cam.begin();

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
	
	cam.end();
	
	ofPopStyle();
}

void testApp::randomiseRays() {
	ray1.randomise();
	ray2.randomise();
	
	intersecter1 = ray1.intersect(ray2);
	intersecter2 = ray2.intersect(ray1);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	randomiseRays();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}