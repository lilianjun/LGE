#pragma once

#include <windows.h>

class CTimer
{
public:
	CTimer() :dbTime(0)
	{
		QueryPerformanceFrequency(&freq);//��ȡ����CPUʱ��Ƶ��
	}

	void Start()
	{
		QueryPerformanceCounter(&startCount);//��ʼ��ʱ
	}

	double Stop()
	{
		QueryPerformanceCounter(&endCount);//ֹͣ��ʱ

		dbTime = ((double)endCount.QuadPart - (double)startCount.QuadPart) / (double)freq.QuadPart;//��ȡʱ���
		return dbTime;
	}

	double Get()const
	{
		return dbTime;
	}

	// ֹͣһ�μ�ʱ�������µļ�ʱ�������ϴ�ʱ�䣬��λ����
	double Frame()
	{
		double time = this->Stop();
		this->Start();
		return time;
	}
private:
	LARGE_INTEGER startCount;//��¼��ʼʱ��

	LARGE_INTEGER endCount;//��¼����ʱ��

	LARGE_INTEGER freq;//����CPUʱ��Ƶ��

	double dbTime; // ��λ����
};
