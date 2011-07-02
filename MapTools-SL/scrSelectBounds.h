//
//  scrSelectBounds.h
//  PC Encode
//
//  Created by Elliot Woods on 10/04/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "scrPointCloud.h"
#include "ofxShader.h"

class scrSelectBounds : public scrPointCloud
{
  	
public:
	scrSelectBounds(string caption);
    
    ofPoint     lbf, rtb;

protected:
	virtual void	drawContent();
    
    ofxShader       shader;
};