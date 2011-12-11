#include "PCApp.h"

#include <cmath>

PCApp::PCApp()
{
	
}

PCApp::~PCApp()
{
}

//--------------------------------------------------------------

void PCApp::setup(){
	
	ofSetVerticalSync(true);
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	_scanner = new PCManager();
	
	//setup scanner
	_scanner->setup();

	_scrTabMain = new scrGroupTabbed("Overview group", 32);
	screens.init(*_scrTabMain);
	
	/////////////////////////////////////////////////////////
	// SCAN GROUP
	/////////////////////////////////////////////////////////
	//
	scrGroupGrid *gridScan = new scrGroupGrid("Scan");
	
	//add the send message group
	scrGroupTabbed *scrMessageGroup = new scrGroupTabbed("Send message");
	scrMessageGroup->push(&_scanner->_encoder->scrSend);
	scrMessageGroup->push(&_scanner->_scrProjectorMask);
	gridScan->push(scrMessageGroup);
	

	gridScan->push(&_scanner->_scrControls);
	for (int iCam=0; iCam<PCConfig().nCameras; iCam++)
	{
		//add projection space preview to the send group
		scrGroupTabbed *scrProjDataGroup = new scrGroupTabbed("Projector data");
		scrProjDataGroup->push(_scanner->_decoder[iCam]->_scrProjectorSpace);
		scrProjDataGroup->push(_scanner->_decoder[iCam]->_scrProjectorNFinds);
		gridScan->push(scrProjDataGroup);
		
		scrGroupTabbed *scrCamDataGroup = new scrGroupTabbed("Camera space");
		scrCamDataGroup->push(_scanner->_decoder[iCam]->_scrCameraSpace);
		scrCamDataGroup->push(_scanner->_decoder[iCam]->_scrCameraNFinds);
		gridScan->push(scrCamDataGroup);
		
		gridScan->push(&_scanner->_decoder[iCam]->_scrCamera);
		
		gridScan->push(_scanner->_decoder[iCam]->_scrHistograms);
        
        gridScan->push(_scanner->_decoder[iCam]->_scrSubScans);
	}
	//
	/////////////////////////////////////////////////////////

	
	
	/////////////////////////////////////////////////////////
	// MAIN TAB GROUP
	/////////////////////////////////////////////////////////
	//
	_scrTabMain->push(gridScan);

#ifndef SCAN_ONLY
	_scrTabMain->push(&_Correlator.scrGridMain);
	_scrTabMain->push(&_Assemble.scrGridMain);
#endif	
	/////////////////////////////////////////////////////////
	
//	tabMain->setBounds(0, 0, ofGetWidth(), ofGetHeight());
	
	ofBackground(20, 20, 20);
}

//--------------------------------------------------------------

void PCApp::update(){
	switch (_scrTabMain->iSelection) {
		case 0:
			//we're looking at scan
			_scanner->update();
			break;
            
#ifndef SCAN_ONLY
		case 1:
			//we're looking at correlate
			_Correlator.update();
			break;
      
		case 2:
			_Assemble.update();
#endif
            break;
			
		default:
			break;
	}
	
	screens.showInterface(_scanner->state==0 || _scrTabMain->iSelection>0);
}

//--------------------------------------------------------------

void PCApp::draw(){
	if (_scanner->state==PC_STATE_SCANNING)
		ofLog(OF_LOG_VERBOSE, "PCApp: drawing interleave frame " + ofToString(Payload::Pointer->iScanInterleaveFrame(_scanner->iFrame)));

	if (_scanner->state==PC_STATE_CALIBRATING)
		ofLog(OF_LOG_VERBOSE, "PCApp: drawing calibration frame " + ofToString(_scanner->iFrame, 0));
}
void PCApp::keyPressed(int key){
	
	switch (key)
	{
		//case 'f': // f = fullscreen
		//	ofSetFullscreen(_screens->doFullscreen());
		//	break;

		case 32: // SPACE = run system
			if (_scanner->state==0)
				_scanner->start();
			else
				_scanner->stop();
		break;
			
		case 'c': // c = calibrate threshold
			_scanner->calibrate();
			break;

		case 's': // s = save current scan data
		{
			string name = ofToString(PCConfig().projWidth) + "," + ofToString(_scanner->interleaveWidth) + "x" +
				ofToString(PCConfig().projHeight) + "," + ofToString(_scanner->interleaveHeight);
			ofFileDialogResult ask = ofSystemSaveDialog(name, "Save file as");
			name = ask.filePath;
			_scanner->save(name);
			break;
		}
		

		case 'v': // v = video settings
			_scanner->videoSettings();
			break;

// currently out of order
//		case 'l': // l = load
//			load();
//			break;

	}
}

//--------------------------------------------------------------
void PCApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void PCApp::mouseMoved(int x, int y ){
	

}

//--------------------------------------------------------------
void PCApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void PCApp::mousePressed(int x, int y, int button){
	

}

//--------------------------------------------------------------
void PCApp::mouseReleased(int x, int y, int button){
}

void PCApp::mouseDown(int x, int y)
{
}

//--------------------------------------------------------------
void PCApp::windowResized(int w, int h){

}


string PCApp::getDateString()
{
	char datestring[10];
	
	time_t now = time(0);
	tm hereandnow=*localtime(&now);
	
	const char format[]="%Y%m%d";
	strftime(datestring, 50, format, &hereandnow);
	
	return string(datestring);
}