#include "Benchmark.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

namespace lpp
{

    BenchmarkResult Benchmark::run(const std::string &name, std::function<void()> func, size_t iterations)
    {
        BenchmarkResult result;
        result.name = name;
        result.iterations = iterations;

        // Warmup
        for (size_t i = 0; i < 10; i++)
        {
            func();
        }

        // Actual benchmark
        auto start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < iterations; i++)
        {
            func();
        }

        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        result.durationMs = duration.count() / 1000.0;
        result.bytesProcessed = 0;
        result.throughputMbps = 0;

        return result;
    }

    void Benchmark::compilerBenchmark(const std::string &sourceFile)
    {
        std::cout << "Running compiler benchmark: " << sourceFile << "\n";

        std::vector<BenchmarkResult> results;

        // Benchmark lexing
        auto lexResult = run("Lexing", [&]()
                             {
                                 // Run lexer
                             },
                             100);
        results.push_back(lexResult);

        // Benchmark parsing
        auto parseResult = run("Parsing", [&]()
                               {
                                   // Run parser
                               },
                               100);
        results.push_back(parseResult);

        // Benchmark transpilation
        auto transpileResult = run("Transpilation", [&]()
                                   {
                                       // Run transpiler
                                   },
                                   100);
        results.push_back(transpileResult);

        printResults(results);
    }

    void Benchmark::runtimeBenchmark(const std::string &executable)
    {
        std::cout << "Running runtime benchmark: " << executable << "\n";

        // Execute and measure time
        auto start = std::chrono::high_resolution_clock::now();
        // Sanitize executable path
        std::string sanitized = executable;
        sanitized.erase(std::remove_if(sanitized.begin(), sanitized.end(),
                                       [](char c)
                                       { return c == '&' || c == '|' || c == ';' || c == '`' || c == '$'; }),
                        sanitized.end());
        system(sanitized.c_str());
        auto end = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Execution time: " << duration.count() << "ms\n";
    }

    void Benchmark::printResults(const std::vector<BenchmarkResult> &results)
    {
        std::cout << "\n=== Benchmark Results ===\n\n";
        std::cout << std::left << std::setw(20) << "Name"
                  << std::right << std::setw(15) << "Duration (ms)"
                  << std::setw(15) << "Iterations"
                  << std::setw(15) << "Avg (ms)" << "\n";
        std::cout << std::string(65, '-') << "\n";

        for (const auto &result : results)
        {
            double avgMs = result.durationMs / result.iterations;
            std::cout << std::left << std::setw(20) << result.name
                      << std::right << std::setw(15) << std::fixed << std::setprecision(2) << result.durationMs
                      << std::setw(15) << result.iterations
                      << std::setw(15) << std::fixed << std::setprecision(4) << avgMs << "\n";
        }
        std::cout << "\n";
    }

    void Benchmark::saveResults(const std::vector<BenchmarkResult> &results, const std::string &outputFile)
    {
        std::ofstream file(outputFile);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open benchmark output file\n";
            return;
        }

        file << "name,duration_ms,iterations,avg_ms\n";
        for (const auto &result : results)
        {
            double avgMs = result.durationMs / result.iterations;
            file << result.name << ","
                 << result.durationMs << ","
                 << result.iterations << ","
                 << avgMs << "\n";
        }

        file.close();
        std::cout << "Benchmark results saved to: " << outputFile << "\n";
    }

    double Benchmark::measureTime(std::function<void()> func)
    {
        auto start = std::chrono::high_resolution_clock::now();
        func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / 1000.0;
    }

} // namespace lpp
