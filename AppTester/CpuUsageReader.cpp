#include "CpuUsageReader.h"


CpuUsageReader::CpuUsageReader(int pid) : m_pid(pid)
{
}

CpuUsageReader::~CpuUsageReader()
{
}

__int64 CpuUsageReader::FileTime2Utc(const FILETIME* ftime)
{
    LARGE_INTEGER li;

    li.LowPart = ftime->dwLowDateTime;
    li.HighPart = ftime->dwHighDateTime;
    return li.QuadPart;
}

int CpuUsageReader::GetProcessorNumber()
{
    SYSTEM_INFO info;
    GetSystemInfo(&info);
    return (int)info.dwNumberOfProcessors;
}

float CpuUsageReader::GetCpuUsage()
{
    //Nr of cores
    static int processor_count_ = -1;
    //Previous time
    static __int64 last_time_ = 0;
    static __int64 last_system_time_ = 0;

    FILETIME now;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    __int64 system_time;
    __int64 time;
    __int64 system_time_delta;
    __int64 time_delta;

    float cpu_usage = -1;

    if (processor_count_ == -1)
    {
        processor_count_ = GetProcessorNumber();
    }

    GetSystemTimeAsFileTime(&now);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, m_pid);

    if (!GetProcessTimes(hProcess, &creation_time, &exit_time, &kernel_time, &user_time))
    {
        return -1;
    }

    system_time = (FileTime2Utc(&kernel_time) + FileTime2Utc(&user_time)) / processor_count_;
    time = FileTime2Utc(&now);

    if ((last_system_time_ == 0) || (last_time_ == 0))
    {
        last_system_time_ = system_time;
        last_time_ = time;
        return -1;
    }

    system_time_delta = system_time - last_system_time_;
    time_delta = time - last_time_;

    if (time_delta == 0)
        return -1;

    cpu_usage = (float)((system_time_delta * 100 + time_delta / 2) / time_delta);
    last_system_time_ = system_time;
    last_time_ = time;

    return cpu_usage;
}