#pragma once
/*
 *  CameraBase.h
 *  PCEncode
 *
 *  Created by Elliot Woods on 16/03/2011.
 *  Copyright 2011 Kimchi and Chips. All rights reserved.
 *
 */

#include "PCConfig.h"

class CameraBase : public ofBaseDraws, public ofBaseHasPixels
{
public:
	CameraBase();
	~CameraBase();

	bool setup(int ID, int width, int height, float delay);
	void updatePreview();
	void capture();
	
	//virtuals
	virtual void videoSettings() {ofLog(OF_LOG_WARNING, "videoSettings not implemented for this camera type");};
	virtual void close() = 0;
	virtual bool isFrameNew() {grab(); return hasWaited(); };
	//ofBaseHasPixels
	unsigned char* getPixels();
	ofPixels&	getPixelsRef();
	
	//ofBaseDraws
	void draw(float x,float y);
	void draw(float x,float y,float w, float h);
	float getHeight();
	float getWidth();

	void	clearTimer();
	bool	hasWaited();
	int		lastTimerClear;

	const int	&ID;

	ofPixels	greyPixels;
	
protected:
	virtual bool	init(int ID, int width, int height) = 0;
	
	virtual void	grab() = 0;
	virtual void	grabPreview() = 0;
	
	ofTexture	*preview;
	int			_ID;
	bool		_initialised;
	
	float		delay;

};