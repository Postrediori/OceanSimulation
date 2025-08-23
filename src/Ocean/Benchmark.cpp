#include "stdafx.h"
#include "Benchmark.h"

namespace Utils {

void OperationBenchmark::Reset() {
    counter = 0;
    totalDuration = 0;
}

void OperationBenchmark::StartOperation() {
    startTime = std::chrono::high_resolution_clock::now();
}

void OperationBenchmark::EndOperation() {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);

    totalDuration += duration.count();
    counter++;
}

uint64_t OperationBenchmark::GetAverage() const {
    if (counter > 0) {
        return totalDuration / counter;
    }
    return 0;
}

} // namespace Utils
