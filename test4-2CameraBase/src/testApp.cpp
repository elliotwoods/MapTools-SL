#include "testApp.h"

#define FOREACH_CAMERA for (int iCam=0; iCam<NUM_CAMERAS; ++iCam)

//--------------------------------------------------------------
void testApp::setup(){
	
	ofBackground(80,80,80);
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	//glEnable(GL_DEPTH_TEST);
	
	if (ofxEdsdk::Camera::listDevices() != NUM_CAMERAS)
	{
		cout << "Wrong camera count!";
		std::exit(1);
	}
	
	FOREACH_CAMERA
		camera[iCam].init(iCam);
	
	intersecter.color = ofColor(255,255,0);
	
	gui.init();
	gui.mainScreen = &mainScreen;
	mainScreen.setGridWidth(1);

	FOREACH_CAMERA
		mainScreen.push(camera[iCam].screens);
	
	FOREACH_CAMERA
		ofAddListener(camera[iCam].calibrationScreen.evtDraw3D, this, &testApp::draw3DCam);

	FOREACH_CAMERA
		ofAddListener(camera[iCam].evtMoveRay, this, &testApp::updateIntersects);
	
	timerOn = false;
	lastCaptureTime = 0;

}

//--------------------------------------------------------------
void testApp::update(){
	
	FOREACH_CAMERA
		camera[iCam].update();
	
	if (NUM_CAMERAS==2)
		if (camera[0].bangAdded && camera[1].bangAdded)
		{
			camera[0].bangAdded = false;
			camera[1].bangAdded = false;
			calcTransforms();
		}
	
	if (timerOn)
		if (ofGetElapsedTimef() > (lastCaptureTime+TIMER_PERIOD) && !camera[0].doAdd && !camera[1].doAdd)
		{
			capture();
			lastCaptureTime = ofGetElapsedTimef();
		}
}

//--------------------------------------------------------------
void testApp::draw(){
	
}

void testApp::updateIntersects(Ray &r) {
	
	Ray r1, r2;
	r1 = camera[0].ray;
	r2 = camera[1].ray;
	r2.transform(matPosRotFromXtoOther[1]);
	
	intersecter = r1.intersect(r2);
	
	cout << "intersect length = " << intersecter.t.length() << endl;
	cout << "intersect midpoint = " << intersecter.s + intersecter.t*0.5 << endl;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if (key=='v')
		for(int i=0; i<2; ++i)
			camera[i].settings();
	
	if (key==' ')
	{
		capture();
	}
	
	if (key=='t')
	{
		timerOn ^= true;
		lastCaptureTime = ofGetElapsedTimef();
	}

	if (key=='c')
		clear();
	
	if (key=='s')
		save();
	
	if (key=='l')
		load();
	
}

void testApp::capture() {
	
	if (NUM_CAMERAS == 2)
	{
		//first check if we've got an asymetry from last time
		int minCalibs = min(camera[0].calibCount(), camera[1].calibCount());
		
		FOREACH_CAMERA
			camera[iCam].shrink(minCalibs);
	}
	
	FOREACH_CAMERA
		camera[iCam].add();
}

void testApp::calcTransforms() {
	if (camera[0].calib.isReady && camera[1].calib.isReady)
	{
		Mat tra, rot;
		
		if (!camera[0].calib.getTransformation(camera[1].calib, rot, tra))
			return;
		
		matPosRotFromXtoOther[0] = makeMatrix(rot, tra);
		
		camera[1].calib.getTransformation(camera[0].calib, rot, tra);
		matPosRotFromXtoOther[1] = makeMatrix(rot, tra);
	}
}

string getAttribName(int iCam, int i, int j) {
	return "Camera" + ofToString(iCam) + "Transform_" + ofToString(i) + "-" + ofToString(j);
}

void testApp::save() {
	FileStorage fs;
	if (NUM_CAMERAS==2)
		fs = FileStorage(ofToDataPath("transforms.xml"), FileStorage::WRITE);
	
	FOREACH_CAMERA {
		if (NUM_CAMERAS==2)
			for (int i=0; i<4; ++i)
				for (int j=0; j<4; ++j)
					fs << getAttribName(iCam,i,j) << matPosRotFromXtoOther[iCam](i, j);
		camera[iCam].save(iCam);
	}
}

void testApp::load() {
	FileStorage fs;
	if (NUM_CAMERAS==2)
		fs = FileStorage(ofToDataPath("transforms.xml"), FileStorage::READ);
	
	FOREACH_CAMERA {
		if (NUM_CAMERAS==2)
			for (int i=0; i<4; ++i)
				for (int j=0; j<4; ++j)
					fs[getAttribName(iCam,i,j)] >> matPosRotFromXtoOther[i](i, j);
		camera[iCam].load(iCam);
	}
}

void testApp::clear() {
	FOREACH_CAMERA
		camera[iCam].clear();
}

void testApp::draw3DCam(ofNode& n) {
	if (NUM_CAMERAS != 2)
		return;
	
	int i;
	if (&n == (ofNode*)&camera[0].calib)
		i = 0;
	else
		i = 1;
	
	ofDrawAxis(10);
	
	ofPushMatrix();
	ofRotate(90,1,0,0);
	ofDrawGrid(100);
	ofPopMatrix();
	
	ofPushMatrix();
	glMultMatrixf(matPosRotFromXtoOther[i].getPtr());
	ofDrawAxis(10);
	ofPopMatrix();
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