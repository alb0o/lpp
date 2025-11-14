#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <any>

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

int sum(int first, std::vector<auto> numbers) {
    return first;
}
int main() {
    auto double = [](auto x) { return (x * 2); };
    auto add = [](auto a, auto b) { return (a + b); };
    auto x = 42;
    auto y = 10;
    auto value = ([&]() { auto __tmp = y; return __tmp != nullptr && __tmp != 0 ? __tmp : 100; })();
    auto numbers = std::vector<auto>{1, 2, 3, 4, 5};
    auto arr1 = std::vector<auto>{1, 2, 3};
    auto arr2 = std::vector<auto>{4, 5, 6};
    auto combined = ([&]() { std::vector<auto> __arr; __arr.insert(__arr.end(), (arr1).begin(), (arr1).end()); __arr.insert(__arr.end(), (arr2).begin(), (arr2).end()); return __arr; })();
    auto result = double(double(x));
    auto triple = [](auto x) { return (x * 3); };
    auto composed = double.triple;
    auto grade = ([&]() { auto __match_0 = x; if (__match_0 == 90) { return "A"; } else if (__match_0 == 80) { return "B"; } else if (__match_0 == 70) { return "C"; } })();
    auto status = ((x > 50) ? "high" : "low");
    auto range = /* range: 1..10 */;
    return 0;
}
