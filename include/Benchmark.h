#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <chrono>
#include <vector>
#include <functional>

namespace lpp
{

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
