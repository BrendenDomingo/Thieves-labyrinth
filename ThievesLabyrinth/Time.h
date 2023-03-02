#pragma once
#include <time.h>
#include <chrono>
//Time
class CTime
{
	static clock_t tTimeStart, tTimeCurrent;
	static double dGameTime;
	static double dDelta;
	static bool	bPaused;
	static double dLast;
public:
	CTime();

	// Updates the time and returns the
	// difference in time in seconds
	// since the last update call
	static void Update();

	// Ensures that Delta is 0
	// This way things don't update very fast
	static void Pause();

	// Prepares the clock to switch back to Delta
	// Resets the clock
	static void Unpause();

	// Get the difference of time in seconds
	static double GetDelta();

	// Get the difference of time in seconds
	static float GetDeltaF();

	// Get the total time the timer has been updating since the last restart call
	static double GetTotalTime();

	// Restart the clock
	static void Restart();

	~CTime();
};

