//
//  CameraHead.h
//  2webcams
//
//  Created by Elliot Woods on 03/09/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

//#define CAM_RES_X 1280
//#define CAM_RES_Y 720
#define CAM_RES_X 640
#define CAM_RES_Y 360

#define MIN_CALIBS 3

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxCVgui.h"

#include "Ray.h"
#include "CameraEdsdk.h"

using namespace cv;
using namespace ofxCv;


class CameraHead : ofThread, ofBaseDraws {
public:
	CameraHead();
	~CameraHead();
	
	void	init(int iDevice);
	void	close(ofEventArgs& e);
	
	void	update();
	void	drawCorners(ofRectangle &r);
	void	drawOnUndistorted(ofRectangle &r);
	void	drawFrustum(ofNode& n);
	
	void	settings();
	void	load(int i);
	void	save(int i);

	void	add();
	void	clear();
	unsigned int	calibCount() { return calib.imagePoints.size(); }
	void			shrink(unsigned int s);
	
	//ofBaseDraws
	void draw(float x,float y);
	void draw(float x,float y,float w, float h);
	float getWidth();
	float getHeight();
	
	scrGroupGrid	screens;
	scrGroupTabbed	cameraScreen;
	scrDraw2D		cameraRawScreen;
	scrDraw2D		cameraGreyScreen;
	scrDraw2D		cameraUndistortedScreen;
	scrDraw3D		calibrationScreen;
	
	ofxCv::Calibration	calib;
	
	bool			bangAdded;
	bool			doAdd;
	ofMatrix4x4		matP;
	ofMatrix4x4		matPInverse;
	Ray				ray;
	
	ofEvent<Ray>	evtMoveRay;
	float			viewScale;
protected:
	void	updateCursor(ofPoint& p);
	
	void	threadedFunction();
	void	threadedAdd();
	
	ofImage	greyThreadedInternal;
	bool	greyFresh;
	
	float		width, height;
	
	CameraEdsdk			*cam;
	ofImage				camUndistorted;
	bool				waitingForFrame;
	ofMesh				frustum;
	
	vector<Point2f>		imagePoints;
	ofMutex				lockCorners;
	

};