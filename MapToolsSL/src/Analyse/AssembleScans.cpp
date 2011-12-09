//
//  AssembleScans.cpp
//  PC Encode
//
//  Created by Elliot Woods on 08/05/2011.
//  Copyright 2011 Kimchi and Chips. All rights reserved.
//

#include "AssembleScans.h"

void TraRot::evaluate(pfitDataPoint<float>& p, const vector<float> &parameters) const {
	ofVec3f &in(*(ofVec3f*)p.getInput());
	ofVec3f &out(*(ofVec3f*)p.getOutput());

	//rotate then translate
	out = in.rotate(parameters[3], parameters[4], parameters[5])
		+ ofVec3f(parameters[0], parameters[1], parameters[2]);
}

AssembleScans::AssembleScans() :
scrGridMain("Assemble scans"),
scrControl("Control"),
scrData("Data", data),
bangAssemble("Assemble scans"),
bangSave("Save assembled"),
ransac_residual(0.03),
ransac_selection(0.1),
ransac_inclusion(0.5),
ransac_iterations(5)
{
    scrSelectFiles = new scrFileSelect("Select scan sets", SCANSET_LOCATION, SCANSET_EXTENSION);
    
    scrGridMain.push(&scrControl);
    scrGridMain.push(scrSelectFiles);
    scrGridMain.push(&scrData);
    
    wdgSlider *sliderResidual = new wdgSlider("Residual threshold", ransac_residual, 0, 0.1, 0.005, "m");
    wdgSlider *sliderSelection = new wdgSlider("Initial selection", ransac_selection, 0, 1, 0.05);    
    wdgSlider *sliderInclusion = new wdgSlider("Inclusion threshold", ransac_inclusion, 0, 1, 0.05);    
    wdgSlider *sliderIterations = new wdgSlider("Iterations", ransac_iterations, 1, 100, 1, "iterations");   
    
    
    scrControl.push(&bangAssemble);
    scrControl.push(sliderResidual);
    scrControl.push(sliderSelection);
    scrControl.push(sliderInclusion);
    scrControl.push(sliderIterations);
    scrControl.push(&bangSave);
}

AssembleScans::~AssembleScans()
{
    delete scrSelectFiles;
}

void AssembleScans::update()
{
    if (bangAssemble.getBang())
        assemble();
    
    if (bangSave.getBang())
    {
        data.save(ofToDataPath("assembled." + SCANSET_EXTENSION));
        data.saveImage("assembled.bmp");
    }
}

void AssembleScans::assemble()
{
    if (scrSelectFiles->nFiles < 2)
        return;
    
    //fill a vector with select filenames
    vector<string> filenames;
    for (int i=0; i<scrSelectFiles->nFiles; i++)
        if (scrSelectFiles->selected[i])
            filenames.push_back(scrSelectFiles->getPath(i));
        
    
    if (filenames.size() < 1)
        return;

	//data is the base
    data.load(filenames[0]);
    
    ScanSet other;
    
    for (int iOther = 1; iOther < filenames.size(); iOther++)
    {
        other.load(filenames[iOther]);
        assemble(data, other);        
    }
}

void AssembleScans::assemble(ScanSet &base, ScanSet &addition)
{
    //check we're working with compatible sets
    if (base.width != addition.width || base.height != addition.height)
    {
        ofLog(OF_LOG_WARNING, "AssembleScans: We cannot assemble these 2 scans as they have different projector dimensions");
        return;
    }
	
	
	
	//////////////////////////
	// FIND COMMON AND
	// DISCRETE POINTS
	// WITHIN THE 2 SETS
	//////////////////////////
	//
    
    //find common and unique points between two sets
    vector<int> commonPointsBase, commonPointsAddition;
    vector<int> uniquePointsAddition;
    
    cout << "AssmbleScans: Finding common points";
    int nCommonPoints = 0;
    
    int iPointAddition;
    bool foundCommon;
    //
    for (iPointAddition=0; iPointAddition < addition.size; iPointAddition++)
    {
        foundCommon = false;
        
        for (int iPointBase=0; iPointBase<base.size; iPointBase++)
        {
            if (base.iX[iPointBase] == addition.iX[iPointAddition] && base.iY[iPointBase] == addition.iY[iPointAddition])
            {
                commonPointsAddition.push_back(iPointAddition);
                commonPointsBase.push_back(iPointBase);
                nCommonPoints++;
                
                foundCommon = true;
                break;
            }
        }
        
        if (!foundCommon)
            uniquePointsAddition.push_back(iPointAddition);
    }
        
    //create fit data to move
    //points from addition to base
	pfitDataSetf dataSet;
	dataSet.init(3, 3, nCommonPoints);
	pfitDataPointf pt = dataSet.begin();
	ofVec3f *input, *output;
    char iDim;
    for (int iPoint = 0; iPoint < nCommonPoints; iPoint++)
    {
		input = (ofVec3f*)pt.getInput();
		output = (ofVec3f*)pt.getOutput();
		
		*input = addition[commonPointsAddition[iPoint]];
		*output = base[commonPointsAddition[iPoint]];
	
		++pt;
    }
	//
	//////////////////////////
    
	
	
	
	//////////////////////////
	// FIT ON COMMON POINTS
	//////////////////////////
	//
    //perform fit
	TraRot model;
	vector<float> pTraRot(6);
	levmar.correlate(dataSet, model, pTraRot, 100);
	//
	//////////////////////////
	
	
	
	
	//////////////////////////
	// SETUP AND EVALUATE
	// NEW DATASET
	// ADDING DISCRETE POINTS
	//////////////////////////
	//
    
    //setup new dataset
    cout << "Found " << nCommonPoints << " common points\n";
    ScanSet combined;
    combined.initialise(base, base.size + uniquePointsAddition.size());
    
    //copy base points into combined set
    memcpy(combined.xyz, base.xyz, base.size * 3 * 4);
    memcpy(combined.iX, base.iX, base.size * 4);
    memcpy(combined.iY, base.iY, base.size * 4);
    
    
    //evaluate addition points in base space
    ofVec3f* combinedMover = combined.xyz + base.size;
    unsigned int * iXMover = combined.iX + base.size;
    unsigned int * iYMover = combined.iY + base.size;
    
    int idxAdditionSet;
    
    cout << "Adding " << uniquePointsAddition.size() << " points to exsting set of " << base.size << ".\n";
    
	pfitDataPointf evalpt;
    for (int i=0; i<uniquePointsAddition.size(); i++)
    {
        
        idxAdditionSet = uniquePointsAddition[i];
        evalpt = pfitDataPointf(3, 3, (float*)&addition.xyz[idxAdditionSet], (float*)combinedMover);
        model.evaluate(evalpt, pTraRot);
		
        *iXMover = addition.iX[idxAdditionSet];
        *iYMover = addition.iY[idxAdditionSet];
        
		++combinedMover;
        ++iXMover;
        ++iYMover;
    }

    //replace base with combined
    base = combined;
	//
	//////////////////////////
}




