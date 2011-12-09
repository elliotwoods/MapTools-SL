#pragma once

#include "ofMain.h"

#include "ray.h"
#include "SecondCamera.h"
class testApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void drawScene();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofRectangle		viewports[2];
	ofEasyCam		cam1;
	SecondCamera	cam2;

	Ray ray1, ray2, intersecter1, intersecter2;

	void setupViewports();
	void randomiseRays();
	void updateIntersects();
	
	bool isMoving;
};
