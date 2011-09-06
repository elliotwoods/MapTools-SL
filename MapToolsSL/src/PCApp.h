#ifndef _PCApp
#define _PCApp


#include "ofMain.h"

#include "ofxCVgui.h"

#include "PCconstants.h"
#include "PCManager.h"

#include "CorrelateMain.h"

//#define SCAN_ONLY

class PCApp : public ofBaseApp {

	public:
		PCApp();
		~PCApp();
	
		void setup();
		void update();
		void draw();
//		void drawStatus();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button); 
		void mouseReleased(int x, int y, int button);
		void mouseDown(int x, int y); // new function. is called whenever mouse is down / moved when down

		void windowResized(int w, int h);
	
	private:
		string				getDateString();
		
		ofxCVgui			screens;
		scrGroupTabbed		*_scrTabMain;

		string				_strStatus;
		char				_strDebug[200];
		string				_strHistograms;
	

protected:

		PCManager			*_scanner;
#ifndef SCAN_ONLY
	//correlator
	CorrelateMain			_Correlator;
#endif
	
	//calibration variables
	float					_screenWidth, _screenHeight;
};

#endif
