//
//  RANSACFilter.h
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKCScreensGUI.h"
#include "ofxPolyFit.h"

#include "scrScanSet.h"

class RANSACFilter
{
    public:
        RANSACFilter();
        ~RANSACFilter();
    
        void            update();
    
        scrGroupGrid    scrGridMain;
    
    protected:    
        void        filterRANSAC();
    
        void        calcRGB();
    
        scrGroupGrid    scrData;
        scrGroupGrid    scrLeftGroup;
        scrScanSet      scrInput, scrFiltered;
        scrFileSelect   scrSelectFile;
    
        wdgButton       *bangLoadScan, *bangRANSAC, *bangSaveFiltered;
        scrWidgets      scrControl;
    
        float           ransac_selection;
        float           ransac_residual;
        float           ransac_inclusion;
        float           ransac_iterations;
        ofxPolyFit      RANSAC;
    
    
        ///////////////////
        // DATA
        ///////////////////
        //
        ScanSet         setInput;
        ScanSet         setFiltered;
        //
        ///////////////////
    
};