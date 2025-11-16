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
        : states(s), probabilities(probs), hasWeights(true), rng(std::chrono::system_clock::now().time_since_epoch().count()) {}
    
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

int main() {
    auto point = std::make_tuple(10, 20, 30);
    auto __tuple_tmp = point;
    auto x = std::get<0>(__tuple_tmp);
    auto y = std::get<1>(__tuple_tmp);
    auto z = std::get<2>(__tuple_tmp);
    print("X:");
    print(x);
    print("Y:");
    print(y);
    print("Z:");
    print(z);
    auto __tuple_tmp = std::make_tuple("Alice", 30, true);
    auto name = std::get<0>(__tuple_tmp);
    auto age = std::get<1>(__tuple_tmp);
    auto active = std::get<2>(__tuple_tmp);
    print("Name:");
    print(name);
    print("Age:");
    print(age);
    auto pair = std::make_tuple(std::make_tuple(1, 2), std::make_tuple(3, 4));
    auto __tuple_tmp = pair;
    auto first = std::get<0>(__tuple_tmp);
    auto second = std::get<1>(__tuple_tmp);
    print("Tuple destructuring works!");
    return 0;
}
