#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <array>
#include <optional>
#include <functional>
#include <variant>
#include <map>
#include <any>
#include <future>

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

enum Status {
    PENDING = 0,
    ACTIVE = 1,
    COMPLETED = 2
};

class Config {
// AUTO-GENERATED: Singleton Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_SINGLETON(Config)
public:
    
    std::string getSetting() {
        return "default";
    }
    
};

class EventManager {
// AUTO-GENERATED: Observer Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_OBSERVER(EventManager)
public:
    
    void notify() {
        print("Notifying observers");
    }
    
};

template<typename T>
T identity(T value) {
    return value;
}
template<typename T, typename U>
void swap(T a, U b) {
    print("Swapping values");
}
std::future<std::string> fetchData() {
    return std::async(std::launch::async, [=]() {
        return "Data loaded";
    });
}
template<typename T>
std::future<T> processAsync(T value) {
    return std::async(std::launch::async, [=]() {
        return value;
    });
}
template<typename T, typename U>
void mapArray(int size) {
    print("Map function with generics");
}
void main() {
    std::vector<int> numbers = std::vector<auto>{1, 2, 3, 4, 5};
    std::array<int, 10> buffer = std::vector<auto>{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::optional<int> maybeNumber = 42;
    std::variant<int, std::string> flexible = 100;
    for (auto num : numbers) {
        print("Number");
    }
    auto counter = 0;
    do {
        counter = (counter + 1);
    } while ((counter < 5));
    try {
        auto risky = 10;
    } catch (const std::exception& error) {
        print("Error caught");
    }
    // Finally block (executed via RAII)
    {
        print("Cleanup completed");
    }
    Status currentStatus = Status.ACTIVE;
    switch (currentStatus) {
        case Status.PENDING:
            print("Status is pending");
            break;
        case Status.ACTIVE:
            print("Status is active");
            break;
        case Status.COMPLETED:
            print("Status is completed");
            break;
        default:
            print("Unknown status");
    }
    auto __tmp = std::vector<auto>{10, 20};
    auto first = __tmp[0];
    auto second = __tmp[1];
    auto __tmp = std::map<std::string, std::any>{{"name", "John"}, {"age", 30}};
    auto name = __tmp["name"];
    auto age = __tmp["age"];
    auto floatNum = 3.14;
    auto intNum = static_cast<int>(floatNum);
    print("All features compiled successfully!");
}
