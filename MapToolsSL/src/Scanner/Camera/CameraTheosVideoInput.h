/*
 *  CameraTheosVideoInput.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/03/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "ofMain.h"

#ifdef TARGET_WIN32

#include "CameraBase.h"
#include "videoInput.h"

class CameraTheosVideoInput : public CameraBase, ofThread
{
	public:
		void	videoSettings();
		void	close();

	protected:
		bool	init(int ID, int width, int height);
		void	grab();
		void	grabPreview();	
		void	threadedFunction();

		videoInput		_grabber;
		ofTexture		_preview;

		ofPixels	rgbPixels;
		bool		fresh;
};

#endif