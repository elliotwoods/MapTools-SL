#pragma once
//
//  SubScanBasic.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 24/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanBase.h"

class SubScanBasic : public SubScanBase
{
    public:
        SubScanBasic(CameraBase& camera, unsigned char& calibrateRangeQualifier, bool *& projectorMask, Histogram &histThresholdRange, int iSubScan);
    
    protected:
        
        //////////////////////////////////
        // VIRTUAL METHODS
        //////////////////////////////////
        //
        unsigned long lookupPiSx(unsigned long iX);
        unsigned long lookupPiSy(unsigned long iY);
        //
        //////////////////////////////////
};