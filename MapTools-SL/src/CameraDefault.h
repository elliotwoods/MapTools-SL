//
//  CameraDefault.h
//  PC Encode
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "CameraBase.h"

class CameraDefault : public CameraBase
{
public:
    bool	setup(int ID);
    void	videoSettings();
    void	close();
    
protected:
    void	grab();
    ofVideoGrabber		_grabber;
};