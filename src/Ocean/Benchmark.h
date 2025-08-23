#pragma once

namespace Utils {

struct OperationBenchmark {
    OperationBenchmark() = default;

    void Reset();

    void StartOperation();
    void EndOperation();

    uint64_t GetAverage() const;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    uint64_t counter{ 0 };
    uint64_t totalDuration{ 0 };
};

} // namespace Utils
