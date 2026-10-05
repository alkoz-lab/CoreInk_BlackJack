#pragma once
#include <cstdint>

// Caches the battery level and re-reads the ADC only when the cached value is stale.
// Pure C++: the voltage source and clock are injected, so it is host-testable.
class BatteryMonitor
{
public:
    using MillivoltSource = int (*)();
    using Clock = uint32_t (*)();

    static constexpr uint32_t DEFAULT_INTERVAL_MS = 60000;
    static constexpr int READS_PER_SAMPLE = 4; // averaged to smooth single-read ADC noise
    static constexpr int EMPTY_MILLIVOLTS = 3300;
    static constexpr int FULL_MILLIVOLTS = 4150;

    BatteryMonitor(MillivoltSource source, Clock clock,
                   uint32_t intervalMs = DEFAULT_INTERVAL_MS);

    // 0..100. Samples on first use, then at most once per interval.
    int percent();
    // Samples now, e.g. before the final Power Off frame or after waking from sleep.
    void forceRefresh();

    static int percentFromMillivolts(int millivolts);

private:
    void sample();

    MillivoltSource source_;
    Clock clock_;
    uint32_t intervalMs_;
    uint32_t sampledAtMs_ = 0;
    int percent_ = 0;
    bool hasSample_ = false;
};
