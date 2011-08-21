//
//  SubScanBasic.cpp
//  PC Encode
//
//  Created by Elliot Woods on 24/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanBasic.h"

SubScanBasic::SubScanBasic(unsigned char *& cameraGrey, unsigned char& calibrateRangeQualifier, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan) :
SubScanBase(cameraGrey, projectorMask, histThresholdRange, iSubScan)
{
    
    SubScanWidth = projWidth;
    SubScanHeight = projHeight; 
    
    allocateLookups();
    renderLookups();
}

unsigned long SubScanBasic::lookupPiSx(unsigned long iX)
{
    return iX;    
}

unsigned long SubScanBasic::lookupPiSy(unsigned long iY)
{
    return iY;
}