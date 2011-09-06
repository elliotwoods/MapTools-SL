#pragma once
//
//  ScanSet.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofMain.h"
#include <fstream>

class ScanSet
{
    public:
        ScanSet();
        ~ScanSet();
    
        void    allocate();
        void    allocate(int count);
        void    deAllocate();

        void    setup(ScanSet &other);
    
        void    calcRGB();
    
        void    loadBin(string filename);
        void    saveBin(string filename);
    
        void    saveImage(string filename);
    
        void    operator=(ScanSet& other);
    
        float           *xyz;
        float           *rgb;
    
        unsigned int    *iX, *iY;
        int             nPoints;
    
        unsigned short  width, height;
    
        bool            hasBounds;
        float           lbf[3], rtb[3];
    
        bool            isAllocated;
    
        string          loadFilename;
    
};