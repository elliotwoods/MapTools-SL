//
//  scrSelectBounds.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "scrPointCloud.h"

class scrSelectBounds : public scrPointCloud
{
  	
public:
	scrSelectBounds(string caption);
    
    ofVec3f			lbf, rtb;

protected:
	virtual void	drawContent();
    
    ofShader		shader;
};