#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <chrono>
#include <vector>
#include <functional>

namespace lpp
{

    // FIX BUG #178: Benchmark timers not properly stopped on exception
    // TODO: Use RAII timer guard for automatic stop
    // - TimerGuard class: Starts timer in constructor, stops in destructor
    // - On exception: Timer auto-stopped, result recorded
    // - No manual stopTimer() needed
    // Example:
    //   {
    //     TimerGuard timer("MyBenchmark");
    //     doWork(); // If exception, timer auto-stops
    //   } // Timer destructor records result
    struct BenchmarkResult
    {
        std::string name;
        double durationMs;
        size_t iterations;
        size_t bytesProcessed;
        double throughputMbps;
    };

    class Benchmark
    {
    public:
        static BenchmarkResult run(const std::string &name, std::function<void()> func, size_t iterations = 1000);

        static void compilerBenchmark(const std::string &sourceFile);
        static void runtimeBenchmark(const std::string &executable);

        static void printResults(const std::vector<BenchmarkResult> &results);
        static void saveResults(const std::vector<BenchmarkResult> &results, const std::string &outputFile);

    private:
        static double measureTime(std::function<void()> func);
    };

} // namespace lpp

#endif // BENCHMARK_H
