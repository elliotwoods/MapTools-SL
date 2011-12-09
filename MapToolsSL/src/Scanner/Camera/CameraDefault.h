//
//  CameraDefault.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CameraBase.h"

class CameraDefault : public CameraBase
{
public:
    void	videoSettings();
    void	close();
    
protected:
    bool	init(int ID, int width, int height);
    void	grab();
	void	grabPreview();
	
    ofVideoGrabber		_grabber;
};