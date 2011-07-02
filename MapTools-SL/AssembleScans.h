//
//  AssembleScans.h
//  PC Encode
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKCScreensGUI.h"
#include "ofxPolyFit.h"

#include "scrScanSet.h"

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
    
        ofxPolyFit  ransac;
        float           ransac_selection;
        float           ransac_residual;
        float           ransac_inclusion;
        float           ransac_iterations;
    
        scrFileSelect   *scrSelectFiles;
        scrWidgets      scrControl;
        scrScanSet      scrData;
    
        wdgButton       bangAssemble, bangSave;
    
};