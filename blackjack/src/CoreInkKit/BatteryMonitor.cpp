#include "BatteryMonitor.hpp"

BatteryMonitor::BatteryMonitor(MillivoltSource source, Clock clock, uint32_t intervalMs)
    : source_(source), clock_(clock), intervalMs_(intervalMs)
{
}

int BatteryMonitor::percent()
{
    // Unsigned subtraction stays correct when millis() wraps around.
    if (!hasSample_ || clock_() - sampledAtMs_ >= intervalMs_)
        sample();
    return percent_;
}

void BatteryMonitor::forceRefresh()
{
    sample();
}

int BatteryMonitor::percentFromMillivolts(int millivolts)
{
    if (millivolts <= EMPTY_MILLIVOLTS)
        return 0;
    if (millivolts >= FULL_MILLIVOLTS)
        return 100;
    return (millivolts - EMPTY_MILLIVOLTS) * 100 / (FULL_MILLIVOLTS - EMPTY_MILLIVOLTS);
}

void BatteryMonitor::sample()
{
    long total = 0;
    for (int read = 0; read < READS_PER_SAMPLE; ++read)
        total += source_();
    percent_ = percentFromMillivolts(static_cast<int>(total / READS_PER_SAMPLE));
    sampledAtMs_ = clock_();
    hasSample_ = true;
}
