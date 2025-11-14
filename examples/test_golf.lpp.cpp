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

void test_range() {
    auto r = ([&]() { std::vector<int> __range; int __start = 0; int __end = 5; int __step = 1; if (__step > 0) { for (int i = __start; i <= __end; i += __step) __range.push_back(i); } else { for (int i = __start; i >= __end; i += __step) __range.push_back(i); } return __range; })();
    print(r);
}
void test_map() {
    auto nums = std::vector<auto>{1, 2, 3};
    auto doubled = ([&]() { std::vector<decltype([](auto x) { return (x * 2); }(std::declval<decltype(nums)::value_type>()))> __result; for (auto& __item : nums) { __result.push_back(([](auto x) { return (x * 2); })(__item)); } return __result; })();
    print(doubled);
}
int main() {
    test_range();
    test_map();
    return 0;
}
