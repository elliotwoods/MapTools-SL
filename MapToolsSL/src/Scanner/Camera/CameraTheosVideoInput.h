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

class CameraTheosVideoInput : public CameraBase
{
	public:
		bool	setup(int ID);
		void	videoSettings();
		void	close();

		unsigned char *rgbPixels;

	protected:
		void	grab();
		videoInput		_grabber;
};

#endif