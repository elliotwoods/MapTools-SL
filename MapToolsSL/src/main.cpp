#include "ofMain.h"
#include "ofVideoGrabber.h"
#include "ofAppGlutWindow.h"
#include "PCApp.h"
#include "PCconstants.h"

#include "GUIConstants.h"

//========================================================================
int main( ){

    ofAppGlutWindow window;
	ofSetupOpenGL(&window,
				  1024,
				  768,
				  OF_WINDOW);			// <-------- setup the GL context
	
	ofRunApp(new PCApp());

}

