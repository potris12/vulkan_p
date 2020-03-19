#include "stdafx.h"
#include "Timer.h"

void Timer::awake()
{
	prev_time_ = std::chrono::high_resolution_clock::now();
	cur_time_ = std::chrono::high_resolution_clock::now();
}

void Timer::update()
{
	cur_time_ = std::chrono::high_resolution_clock::now();

	dt_ = std::chrono::duration<float, std::chrono::seconds::period>(cur_time_ - prev_time_).count();
	prev_time_ = cur_time_;
}

Timer::Timer()
	: CSingleTonBase<Timer>("Timer")
{
}

Timer::~Timer()
{
}
