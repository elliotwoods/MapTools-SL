//
//  RANSACFilter.cpp
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include "RANSACFilter.h"

RANSACFilter::RANSACFilter() :
scrGridMain("RANSAC filter"),
scrControl("Controls"),
scrData("Data points"),
scrInput("Input data", setInput),
scrFiltered("Filtered data", setFiltered),
scrLeftGroup("Controls"),
scrSelectFile("Choose scan", "Logs", "scan"),
ransac_residual(0.05),
ransac_selection(0.1),
ransac_inclusion(0.2),
ransac_iterations(5)
{
    
    scrGridMain.push(&scrLeftGroup);
    scrGridMain.push(&scrData);
    
    scrLeftGroup.push(&scrControl);
    scrLeftGroup.push(&scrSelectFile);
    
    scrData.setGridWidth(1);
    scrData.push(&scrInput);
    scrData.push(&scrFiltered);
    
    bangLoadScan = new wdgButton("Load scan");
    bangRANSAC = new wdgButton("Perform RANSAC filter");
    bangSaveFiltered = new wdgButton("Save filtered scan");
    
    wdgSlider *sliderResidual = new wdgSlider("Residual threshold", ransac_residual, 0, 0.1, 0.005);
    wdgSlider *sliderSelection = new wdgSlider("Initial selection", ransac_selection, 0, 1, 0.05);    
    wdgSlider *sliderInclusion = new wdgSlider("Inclusion threshold", ransac_inclusion, 0, 1, 0.05);    
    wdgSlider *sliderIterations = new wdgSlider("Iterations", ransac_iterations, 1, 100, 1, "iterations");        
    
    scrControl.push(bangLoadScan);
    scrControl.push(bangRANSAC);
    scrControl.push(bangSaveFiltered);
    scrControl.push(sliderResidual);
    scrControl.push(sliderSelection);
    scrControl.push(sliderInclusion);
    scrControl.push(sliderIterations);
    
}

RANSACFilter::~RANSACFilter()
{

}

void RANSACFilter::update()
{
    if (bangLoadScan->getBang())
    {
        string filename = scrSelectFile.getSelectedPath(); 
        if (filename == "")
        {
            ofLog(OF_LOG_WARNING, "RANSACfilter: No file selected");
            return;
        } else
            setInput.loadBin(filename);
    }
    
    if (bangRANSAC->getBang())
        filterRANSAC();
 
    if (bangSaveFiltered->getBang())
    {
        setFiltered.saveBin(setInput.loadFilename + "-filtered.scan");
        setFiltered.saveImage(setInput.loadFilename + "-filtered.bmp");
    }
}

void RANSACFilter::filterRANSAC()
{
    RANSAC.init(1, 4, 2, BASIS_SHAPE_PADE_FIRST);
    
    double* input = new double[setInput.nPoints * 4];
    double* output = new double[setInput.nPoints * 2];

    double ProjectorX, ProjectorY;
    
    double* inputMove = input;
    double* outputMove = output;
    
    //fill RANSAC
    for (int i=0; i<setInput.nPoints; i++)
    {
        ProjectorX = 2 * (double(setInput.iX[i]) / double(setInput.width)) - 1;
        ProjectorY = 1 - 2 * (double(setInput.iY[i]) / double(setInput.height));
        
        
        inputMove[0] = setInput.xyz[i*3 + 0];
        inputMove[1] = setInput.xyz[i*3 + 1];
        inputMove[2] = setInput.xyz[i*3 + 2];        
        inputMove[3] = ProjectorX;
        
        outputMove[0] = ProjectorX;
        outputMove[1] = ProjectorY;
        
        inputMove += 4;
        outputMove += 2;
    }
    
	/**TODO**/
    //RANSAC.RANSAC(input, output, setInput.nPoints, ransac_iterations, ransac_selection, ransac_residual, ransac_inclusion);
    
    set<int>::iterator it;
    
    setFiltered.setup(setInput);
    setFiltered.allocate(RANSAC.bestConsensus.size());

    
    
    int i=0;
    for (it = RANSAC.bestConsensus.begin(); it != RANSAC.bestConsensus.end(); it++)
    {
        for (int iDim=0; iDim<3; iDim++)
        {
            setFiltered.xyz[i*3 + iDim] = setInput.xyz[*it * 3 + iDim];
            setFiltered.rgb[i*3 + iDim] = setInput.rgb[*it * 3 + iDim];
        }
        
        setFiltered.iX[i] = setInput.iX[*it];
        setFiltered.iY[i] = setInput.iY[*it];
        
        i++;
    }
    
}
