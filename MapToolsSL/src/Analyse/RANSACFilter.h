//
//  RANSACFilter.h
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxCVgui.h"
#include "ofxPolyFit.h"

#include "ScanSet.h"

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
		scrDraw3D		scrInput, scrFiltered;
	
        scrFileSelect   scrSelectFile;
    
        wdgButton       *bangLoadScan, *bangRANSAC, *bangSaveFiltered;
        scrWidgets      scrControl;
    
        float           ransac_selection;
        float           ransac_residual;
        float           ransac_inclusion;
        float           ransac_iterations;
        ofxPolyFitd		RANSAC;
		pfitDataSetd	dataSet;
    
    
        ///////////////////
        // DATA
        ///////////////////
        //
        ScanSet         setInput;
        ScanSet         setFiltered;
        //
        ///////////////////
    
};