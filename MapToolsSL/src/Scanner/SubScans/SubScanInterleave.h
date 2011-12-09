//
//  SubScanInterleave.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 02/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanBase.h"

class SubScanInterleave : public SubScanBase
{
public:
    SubScanInterleave(CameraBase& camera, unsigned char& calibrateRangeQualifier, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan);
    
protected:
    
    //////////////////////////////////
    // VIRTUAL METHODS
    //////////////////////////////////
    //
    unsigned long lookupPiSx(unsigned long iX);
    unsigned long lookupPiSy(unsigned long iY);
    //
    //////////////////////////////////
    
    unsigned long interleaveX;
    unsigned long interleaveY;
};