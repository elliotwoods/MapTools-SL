#pragma once
//
//  scrScanSet.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"

#include "scrBase.h"
#include "ScanSet.h"

class scrScanSet : public scrBase {
	
public:
	scrScanSet(string caption, ScanSet &set);
	~scrScanSet();
	
	void			mouseDragged(int x, int y, int dx, int dy, int button);
	int				pointSize;
    
	static ofPoint	spin;
	static ofPoint	translate;
	static float	distance;
    
protected:
	virtual void	drawContent();
    
    void            begin();
    void            drawPoints();
    void            end();

    
    ScanSet         &_set;
    
	GLuint			vbo[2];	
	
    int             _viewport_temp[4];
};