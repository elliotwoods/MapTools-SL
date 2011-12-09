//
//  LongImage.h
//  MapTools Structured Light
//
//  Created by Elliot Woods on 10/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "ofxCVgui.h"

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
    
        scrDraw2D		scrPreview;
//        scrScanSet      scrData;
    
        wdgButton       bangAdd, bangSave, bangClear;
    
        int             projWidth, projHeight;
        int             nProjectors;
        ofImage         longImage;
        
};