#pragma once

#include <Windows.h>

class CpuUsageReader
{
public:
	CpuUsageReader(int pid);
	~CpuUsageReader();

	float GetCpuUsage();

private:
	static __int64 FileTime2Utc(const FILETIME* ftime);
	static int GetProcessorNumber();

private:
	int m_pid;
};
