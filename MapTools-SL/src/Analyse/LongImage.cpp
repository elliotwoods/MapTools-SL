//
//  LongImage.cpp
//  PC Encode
//
//  Created by Elliot Woods on 10/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "LongImage.h"

LongImage::LongImage() :
scrMainGrid("Long Image"),
scrPreview(longImage.getTextureReference(), "Output"),
selectFiles("Select scan(s) to add", "Logs", "scan"),
bangAdd("Add scans to image"),
bangSave("Save long image"),
bangClear("Clear long image"),
scrControlGrid("Controls"),
scrControls("Controls"),
nProjectors(0)
{
    scrControlGrid.push(&scrControls);
    
}

void LongImage::update()
{
    if (bangAdd.getBang())
        add();
    
    if (bangClear.getBang())
        clear();
    
    if (bangSave.getBang())
        save();
}

void LongImage::add()
{
    int oldLongImageCount = nProjectors;
    int oldWidth = projWidth * oldLongImageCount;
    nProjectors++;
    int newWidth = projWidth * nProjectors;    
    
    //setup images
    unsigned char * oldImage = new unsigned char[projWidth*projHeight*3*oldLongImageCount];
    memcpy(oldImage, longImage.getPixels(), projWidth*projHeight*3*oldLongImageCount);
    unsigned char * newImage = longImage.getPixels();
    
    memset(newImage, 0, 3*projWidth*projHeight*nProjectors);
    
    

    
    //copy old contents around
    for (int j = 0; j < projHeight; j++)
        memcpy(newImage + newWidth*3*j, oldImage + oldWidth*3*j, oldWidth * 3);
    
    /*
    
    //////////////////////////
    // copy new contents in
    //////////////////////////
    //	
    ofPoint& lbf(scrTestCorrelate.lbf);
    ofPoint& rtb(scrTestCorrelate.rtb);
    
	int iPP, iLIP;
	unsigned char col[3];
    float* point;
	
	for (int iPoint=0; iPoint<nPoints; iPoint++)
	{
        point = test_pos[iPoint];
        
        //check if not within selected bounds
        if (point[0] < lbf.x || point[1] < lbf.y || point[2] < lbf.z || point[0] > rtb.x || point[1] > rtb.y || point[2] > rtb.z)
            continue;
        
		//convert position to colour values
		col[0] = ofMap(test_pos[iPoint][0],lbf.x,rtb.x,0,255,true);
		col[1] = ofMap(test_pos[iPoint][1],lbf.y,rtb.y,0,255,true);
		col[2] = ofMap(test_pos[iPoint][2],lbf.z,rtb.z,0,255,true);
		
		iPP = dataset_iPX[iPoint] + projWidth * dataset_iPY[iPoint];
        
		if (iPP < int(projWidth*projHeight) && iPP>=0)
        {
            //put pixels at right of image
            iLIP =  dataset_iPX[iPoint] + newWidth * dataset_iPY[iPoint] + oldWidth;
            
			memcpy(newImage + 3*iLIP, col, 3);
        }
	}
    //
    //////////////////////////
    
    
    //delete old image and reassign
    if (longImageCount > 0)
        delete[] longImage;
    longImage = newImage;
     
     //*/
}

void LongImage::save()
{
}

void LongImage::clear()
{
    
}