#pragma once
#include "Singleton.h"

class Timer : public CSingleTonBase<Timer>
{
public:
	void awake();
	void start() {};
	void update();
	void destroy() { ReleseInstance(); };

	std::chrono::time_point<std::chrono::steady_clock> prev_time_;
	std::chrono::time_point<std::chrono::steady_clock> cur_time_;

	float getDeltaTime() { return dt_; }
private:
	float dt_ = 0.f;

public:
	Timer();
	~Timer();
};
#define TIMER Timer::GetInstance()