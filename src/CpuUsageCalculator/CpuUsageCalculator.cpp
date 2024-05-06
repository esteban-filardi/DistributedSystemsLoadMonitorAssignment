#include "CpuUsageCalculator.h"
#include <chrono>
#include <thread>
#include "../../include/linux-system-usage.hpp"

using namespace get_system_usage_linux;

float CpuUsageCalculator::GetPercentageCpu()
{
    CPU_stats t1 = read_cpu_data();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    CPU_stats t2 = read_cpu_data();

    return 100.f * get_cpu_usage(t1, t2);
}
