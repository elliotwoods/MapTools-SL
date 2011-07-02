//
//  SubScanSet.cpp
//  PC Encode
//
//  Created by Elliot Woods on 22/04/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "SubScanSet.h"

#define FOREACH_SCAN for (vector<SubScanBase*>::iterator it = Scans.begin(); it != Scans.end(); it++)

SubScanSet::SubScanSet() :
screens("SubScans")
{
    screens.setGridHeight(1);
}

void SubScanSet::push(SubScanBase* subScan)
{
    Scans.push_back(subScan);
    
    screens.push(&subScan->screensThreshold);
    screens.push(&subScan->screensFrameData);
}

void SubScanSet::deleteAll(){
    while (Scans.size() > 0)
    {
        delete Scans.back();
        Scans.pop_back();
    }
}

void SubScanSet::initCalibration()
{
    FOREACH_SCAN
        (*it)->initCalibration();
}

void SubScanSet::addCalibrationLow()
{
    FOREACH_SCAN
        (*it)->addCalibrationLow();
}

void SubScanSet::calcCalibration()
{
    FOREACH_SCAN
        (*it)->calcCalibration();
}

void SubScanSet::clearData()
{
    FOREACH_SCAN
        (*it)->clearData();
}

void SubScanSet::updateThresholdSelection(int selection)
{
    FOREACH_SCAN
        (*it)->updateThresholdSelection(selection);
}

void SubScanSet::calcBinary()
{
    FOREACH_SCAN
        (*it)->calcBinary();
    
}

SubScanBase*& SubScanSet::operator[] (unsigned i)
{
    return Scans[i];
}