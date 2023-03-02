#pragma once
#include "Time.h"


clock_t CTime::tTimeStart;
clock_t CTime::tTimeCurrent;
double CTime::dDelta;
double CTime::dLast;
bool CTime::bPaused;
double CTime::dGameTime;

CTime::CTime()
{
	Restart();
}


float CTime::GetDeltaF()
{
	return (float)dDelta;
}

double CTime::GetTotalTime()
{
	return dGameTime;
}

void CTime::Restart()
{
	dDelta = dLast = dGameTime = 0.0;
	tTimeStart = clock();
	bPaused = false;
}

CTime::~CTime()
{

}

void CTime::Update()
{
	if (!bPaused)
	{
		tTimeCurrent = clock();
		double dDiff = double(tTimeCurrent - tTimeStart) / CLOCKS_PER_SEC;
	
		dDelta = dDiff - dLast;

		dLast = dDiff;

		dGameTime += dDelta;
	}
}

double CTime::GetDelta()
{
	return dDelta;
}

void CTime::Pause()
{
	dDelta = dLast = 0.0;
	bPaused = true;
}

void CTime::Unpause()
{
	dLast = 0;
	tTimeStart = clock();
	bPaused = false;
	Update();
}
