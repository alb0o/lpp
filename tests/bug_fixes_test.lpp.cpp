#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <tuple>
#include <array>
#include <optional>
#include <functional>
#include <variant>
#include <map>
#include <any>
#include <future>
#include <random>
#include <chrono>

// ============ QUANTUM VARIABLE SYSTEM ============
template<typename T>
class QuantumVar {
private:
    std::vector<T> states;
    std::vector<double> probabilities;
    std::optional<T> collapsed;
    bool hasWeights;
    std::mt19937 rng;

public:
    // Constructor for uniform distribution
    QuantumVar(const std::vector<T>& s) 
        : states(s), hasWeights(false), rng(std::chrono::system_clock::now().time_since_epoch().count()) {
        // Equal probability for all states
        double prob = 1.0 / states.size();
        probabilities = std::vector<double>(states.size(), prob);
    }
    
    // Constructor for weighted distribution
    QuantumVar(const std::vector<T>& s, const std::vector<double>& probs)
        : states(s), hasWeights(true), rng(std::chrono::system_clock::now().time_since_epoch().count()) {
            // Normalize probabilities to sum to 1.0
            double sum = 0.0;
            for (double p : probs) sum += p;
            if (sum > 0.0) {
                for (double p : probs) probabilities.push_back(p / sum);
            } else {
                // Fallback to uniform if all probabilities are 0
                double uniformProb = 1.0 / states.size();
                probabilities = std::vector<double>(states.size(), uniformProb);
            }
        }
    
    // observe(): Collapse superposition to single state
    T observe() {
        if (!collapsed) {
            // Weighted random selection
            std::discrete_distribution<> dist(probabilities.begin(), probabilities.end());
            int idx = dist(rng);
            collapsed = states[idx];
        }
        return *collapsed;
    }
    
    // map(): Transform all states (lazy)
    template<typename F>
    auto map(F func) -> QuantumVar<decltype(func(std::declval<T>()))> {
        using U = decltype(func(std::declval<T>()));
        std::vector<U> newStates;
        for (const auto& state : states) {
            newStates.push_back(func(state));
        }
        return QuantumVar<U>(newStates, probabilities);
    }
    
    // reset(): Return to superposition
    void reset() {
        collapsed.reset();
    }
    
    // entangle(): Create correlated quantum variable
    template<typename F>
    auto entangle(F transform) -> QuantumVar<decltype(transform(std::declval<T>()))> {
        using U = decltype(transform(std::declval<T>()));
        // If already collapsed, entangled var uses same index
        std::vector<U> entangledStates;
        for (const auto& state : states) {
            entangledStates.push_back(transform(state));
        }
        auto result = QuantumVar<U>(entangledStates, probabilities);
        if (collapsed) {
            // Force same index selection
            int collapsedIdx = 0;
            for (size_t i = 0; i < states.size(); ++i) {
                if (states[i] == *collapsed) { collapsedIdx = i; break; }
            }
            result.collapsed = entangledStates[collapsedIdx];
        }
        return result;
    }
};

// Global entangle function
template<typename T, typename F>
auto entangle(QuantumVar<T>& qvar, F transform) {
    return qvar.entangle(transform);
}
// ================================================

void print(const std::string& s) {
    std::cout << s << std::endl;
}

void print(int n) {
    std::cout << n << std::endl;
}

void print(double n) {
    std::cout << n << std::endl;
}

// Higher-order function: map
template<typename T, typename F>
auto map(const std::vector<T>& vec, F func) {
    std::vector<decltype(func(vec[0]))> result;
    for (const auto& item : vec) {
        result.push_back(func(item));
    }
    return result;
}

// Higher-order function: filter
template<typename T, typename F>
std::vector<T> filter(const std::vector<T>& vec, F predicate) {
    std::vector<T> result;
    for (const auto& item : vec) {
        if (predicate(item)) {
            result.push_back(item);
        }
    }
    return result;
}

// Higher-order function: reduce/fold
template<typename T, typename F>
T reduce(const std::vector<T>& vec, T init, F func) {
    T result = init;
    for (const auto& item : vec) {
        result = func(result, item);
    }
    return result;
}

void testStringEscapes() {
    auto newline = "Hello
World";
    auto tab = "Name:	Value";
    auto quote = "He said "Hello"";
    auto backslash = "Path: C:\Users\test";
    auto mixed = "Line1
	Indented "quote" with \ backslash";
}
void testListComprehension() {
    auto squares = ([&]() { std::vector<decltype((x * x))> __comp_0; for (auto x = 0; x <= 10; ++x) { __comp_0.push_back((x * x)); } return __comp_0; })();
    auto filtered = ([&]() { std::vector<decltype((x * 2))> __comp_1; for (auto x = 0; x <= 20; ++x) { if ((x > 5)) { __comp_1.push_back((x * 2)); } } return __comp_1; })();
}
void testQuantumNormalization() {
    QuantumVar<int> q1({1, 2, 3});
    QuantumVar<int> q2({10, 20, 30});
    QuantumVar<int> q3({1, 2, 3}, {0.1, 0.2, 0.3});
    QuantumVar<int> q4({10, 20, 30}, {5, 10, 15});
    auto val1 = q1.observe();
    auto val2 = q2.observe();
    auto val3 = q3.observe();
    auto val4 = q4.observe();
}
int testBasicFunction(int a, int b) {
    return (a + b);
}
int main() {
    testStringEscapes();
    testListComprehension();
    testQuantumNormalization();
    auto result = testBasicFunction(5, 10);
    return 0;
}
