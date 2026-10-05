#include "../blackjack/src/CoreInkKit/BatteryMonitor.hpp"
#include "check.hpp"

namespace
{
    int millivolts = 4000;
    int reads = 0;
    uint32_t now = 0;

    int readMillivolts()
    {
        ++reads;
        return millivolts;
    }

    uint32_t clock() { return now; }
}

int main()
{
    require(BatteryMonitor::percentFromMillivolts(3000) == 0, "below empty clamps to 0");
    require(BatteryMonitor::percentFromMillivolts(3300) == 0, "empty is 0");
    require(BatteryMonitor::percentFromMillivolts(3725) == 50, "midpoint is 50");
    require(BatteryMonitor::percentFromMillivolts(4150) == 100, "full is 100");
    require(BatteryMonitor::percentFromMillivolts(4300) == 100, "above full clamps to 100");

    BatteryMonitor monitor(readMillivolts, clock, 60000);
    require(reads == 0, "nothing is read before first use");
    require(monitor.percent() == 82, "first use samples immediately");
    require(reads == BatteryMonitor::READS_PER_SAMPLE, "one sample averages several reads");

    millivolts = 3725;
    now = 59999;
    require(monitor.percent() == 82 && reads == BatteryMonitor::READS_PER_SAMPLE,
            "cached value is reused within the interval");
    now = 60000;
    require(monitor.percent() == 50 && reads == 2 * BatteryMonitor::READS_PER_SAMPLE,
            "stale value is re-sampled");

    millivolts = 4150;
    monitor.forceRefresh();
    require(monitor.percent() == 100 && reads == 3 * BatteryMonitor::READS_PER_SAMPLE,
            "forced refresh samples immediately and restarts the interval");

    now = 0xFFFFFFF0u;
    monitor.forceRefresh();
    now = 0x10; // millis() wrapped around 32 ms later
    millivolts = 3300;
    require(monitor.percent() == 100, "wrap-around does not trigger an early sample");
    now = 0xFFFFFFF0u + 60000u;
    require(monitor.percent() == 0, "wrap-around still honours the interval");

    std::puts("All battery monitor tests passed.");
    return EXIT_SUCCESS;
}