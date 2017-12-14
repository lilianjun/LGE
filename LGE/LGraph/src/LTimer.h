#pragma once

#include <windows.h>

class CTimer
{
public:
	CTimer() :dbTime(0)
	{
		QueryPerformanceFrequency(&freq);//获取主机CPU时钟频率
	}

	void Start()
	{
		QueryPerformanceCounter(&startCount);//开始计时
	}

	double Stop()
	{
		QueryPerformanceCounter(&endCount);//停止计时

		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart;//获取时间差
		return dbTime;
	}

	double Get()const
	{
		return dbTime;
	}

	// 停止一次计时，开启新的计时，返回上次时间，单位是秒
	double Frame()
	{
		double time = this->Stop();
		this->Start();
		return time;
	}
private:
	LARGE_INTEGER startCount;//记录开始时间

	LARGE_INTEGER endCount;//记录结束时间

	LARGE_INTEGER freq;//本机CPU时钟频率

	double dbTime; // 单位是秒
};
