#include "timerclass.h"


TimerClass::TimerClass()
{
}

TimerClass::TimerClass(const TimerClass& other)
{
}

TimerClass::~TimerClass()
{
}

bool TimerClass::Initialize()
{
	//check if this system supports high performance timers
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0)
	{
		return false;
	}

	//find out how many times the frequency counter ticks every millisecond
	m_ticksPerMs = (float)(m_frequency / 1000); //m_frequency is in sec, convert to millisec
	
	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);

	return true;
}

void TimerClass::Frame() //calculate the difference of time between loops and determine the time it took to execute this frame, then store the current time as the start of the next frame.
{
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

float TimerClass::GetTime() //returns the most recent frame time that was calculated
{
	return m_frameTime;
}