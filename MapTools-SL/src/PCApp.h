#ifndef _PCApp
#define _PCApp


#include "ofMain.h"

#include "ofxKCScreensGUI.h"

#include "PCconstants.h"
#include "PCManager.h"

#include "CorrelateMain.h"
#include "RANSACFilter.h"
#include "AssembleScans.h"

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
		
		PCManager			_scanner;

	
		ofxKCScreensGUI		*_screens;
		scrGroupTabbed		*_scrTabMain;

		string				_strStatus;
		char				_strDebug[200];
		string				_strHistograms;
	

protected:

#ifndef TARGET_WIN32
	//correlator
	CorrelateMain			_Correlator;
    
    //process
    RANSACFilter            _Processor;
    AssembleScans           _Assembler;
#endif
	
	//calibration variables
	float					_screenWidth, _screenHeight;
};

#endif
