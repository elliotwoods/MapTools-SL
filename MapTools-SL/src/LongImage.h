//
//  LongImage.h
//  PC Encode
//
//  Created by Elliot Woods on 10/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "ofxKCScreensGUI.h"
#include "scrScanSet.h"

class LongImage
{
    public:
        LongImage();
    
        void            update();
        scrGroupGrid    scrMainGrid;
    
    protected:
        void            add();
        void            save();
        void            clear();
    
        scrGroupGrid    scrControlGrid;
        scrWidgets      scrControls;
        scrFileSelect   selectFiles;
    
        scrTexture      scrPreview;
//        scrScanSet      scrData;
    
        wdgButton       bangAdd, bangSave, bangClear;
    
        int             projWidth, projHeight;
        int             nProjectors;
        ofImage         longImage;
        
};