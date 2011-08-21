#pragma once
//
//  SubScanSet.h
//  PC Encode
//
//  Created by Elliot Woods on 22/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanBase.h"
#include "SubScanBasic.h"
#include "SubScanInterleave.h"

enum SubScanType
{
    SS_Basic,
    SS_Interleave,
    SS_Projector
};

class SubScanSet
{
    public:
        SubScanSet();
        void    push(SubScanBase* subScan);
        void    deleteAll();
    
        void    initCalibration();
        void    addCalibrationLow();
        void    calcCalibration();
    
        void    clearData();
    
        void    updateThresholdSelection(int selection);
    
        void    calcBinary(); // apply threshold to create binary image
    
        SubScanBase*&   operator[](unsigned i);
    
        vector<SubScanBase*>    Scans;
    
        scrGroupGrid            screens;
    
};