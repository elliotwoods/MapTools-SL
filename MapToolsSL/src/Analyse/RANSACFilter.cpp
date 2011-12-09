//
//  RANSACFilter.cpp
//  PC Encode
//
//  Created by Elliot Woods on 06/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
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
    
    scrGridMain.push(scrLeftGroup);
    scrGridMain.push(scrData);
    
    scrLeftGroup.push(scrControl);
    scrLeftGroup.push(scrSelectFile);
    
    scrData.setGridWidth(1);
    scrData.push(scrInput);
    scrData.push(scrFiltered);
    
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
	//pade shaped fit
    RANSAC.init(1, 4, 2, BASIS_SHAPE_PADE_FIRST);
	
    double ProjectorX, ProjectorY;
    
	dataSet.init(4, 2, setInput.nPoints);
	pfitDataPointd pt = dataSet.begin();

	double *input, *output;
    for (int i=0; i<setInput.nPoints; i++)
    {
		
        ProjectorX = 2 * (double(setInput.iX[i]) / double(setInput.width)) - 1;
        ProjectorY = 1 - 2 * (double(setInput.iY[i]) / double(setInput.height));
        
		input = pt.getInput();
        output = pt.getOutput();
		
        input[0] = setInput.xyz[i].x;
        input[1] = setInput.xyz[i].y;
        input[2] = setInput.xyz[i].z;        
        input[3] = ProjectorX;
        
        output[0] = ProjectorX;
        output[1] = ProjectorY;
		
		++pt;
	}
    
	RANSAC.RANSAC(dataSet, ransac_iterations, ransac_selection, ransac_residual, ransac_inclusion);
    
	
	//setup output dataset and copy in filtered points
	int nPointsOutput = dataSet.getActiveCount();
    setFiltered.setup(setInput);
    setFiltered.allocate(nPointsOutput);
    
	pfitIndexSet::iterator it;
	pfitIndexSet s = dataSet.getActiveIndices();
    int iOutput=0;
    for (it=s.begin(); it != s.end(); ++it) {
		
		setFiltered.xyz[iOutput] = setInput.xyz[*it];
        
        setFiltered.iX[iOutput] = setInput.iX[*it];
        setFiltered.iY[iOutput] = setInput.iY[*it];
        
        ++iOutput;
    }
    
}
