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

class CameraBase : public PCConfig, public ofThread
{
public:
	CameraBase();
	~CameraBase();

	virtual bool setup(int ID) = 0;
	bool	capture(unsigned char*& pixels, bool forceWait=false);
	virtual void videoSettings() {ofLog(OF_LOG_WARNING, "videoSettings not implemented for this camera type");};
	virtual void close() = 0;

	void	threadedFunction();

	void	clearTimer();
	bool	hasWaited();
	int		lastTimerClear;

	const int	&ID;

	unsigned char *greyPixels;


protected:
	virtual void	grab() = 0;
	int			_ID;
	bool		_initialised;

};