//
//  SubScanInterleave.cpp
//  MapTools Structured Light
//
//  Created by Elliot Woods on 02/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.//

#include "SubScanInterleave.h"

SubScanInterleave::SubScanInterleave(CameraBase& camera, unsigned char& calibrateRangeQualifier, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan) :
SubScanBase(camera, projectorMask, histThresholdRange, iSubScan)
{
    
    SubScanWidth = projWidth / interleaveWidth;
    SubScanHeight = projHeight / interleaveHeight; 
    
    interleaveX = _iSubScan % interleaveWidth;
    interleaveY = _iSubScan / interleaveWidth;
    
    allocateLookups();
    renderLookups();
}

unsigned long SubScanInterleave::lookupPiSx(unsigned long iX)
{
    return iX * interleaveWidth + interleaveX;    
}

unsigned long SubScanInterleave::lookupPiSy(unsigned long iY)
{
    return iY * interleaveHeight + interleaveY;
}