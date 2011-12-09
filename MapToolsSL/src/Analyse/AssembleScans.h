#pragma once
//
//  AssembleScans.h
//  PC Encode
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxCVgui.h"
#include "ofxLevMar.h"

#include "ScanSet.h"

using namespace ofxLevMar;

class TraRot : public Model<float> {
public:
	void evaluate(pfitDataPoint<float>& p, const vector<float> &parameters) const;
};

class AssembleScans
{
    public:
        AssembleScans();
        ~AssembleScans();
    
        void        update();
        
        scrGroupGrid    scrGridMain;
    
    protected:
        void        assemble();
        void        assemble(ScanSet &base, ScanSet &addition);
    
        ScanSet     data;
 
        float           ransac_selection;
        float           ransac_residual;
        float           ransac_inclusion;
        float           ransac_iterations;
    
        scrFileSelect   *scrSelectFiles;
        scrWidgets      scrControl;
        scrDraw3D		scrData;
    
        wdgButton       bangAssemble, bangSave;
    
		Fit<float>		levmar;
};