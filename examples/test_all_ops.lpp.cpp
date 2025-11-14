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

int main() {
    auto r1 = ([&]() { std::vector<int> __range; int __start = 0; int __end = 5; int __step = 1; if (__step > 0) { for (int i = __start; i <= __end; i += __step) __range.push_back(i); } else { for (int i = __start; i >= __end; i += __step) __range.push_back(i); } return __range; })();
    auto r2 = ([&]() { std::vector<int> __range; int __start = 1; int __end = 10; int __step = 2; if (__step > 0) { for (int i = __start; i <= __end; i += __step) __range.push_back(i); } else { for (int i = __start; i >= __end; i += __step) __range.push_back(i); } return __range; })();
    auto nums = std::vector<auto>{1, 2, 3, 4, 5};
    auto doubled = ([&]() { std::vector<decltype([](auto x) { return (x * 2); }(std::declval<decltype(nums)::value_type>()))> __result; for (auto& __item : nums) { __result.push_back(([](auto x) { return (x * 2); })(__item)); } return __result; })();
    auto sum = ([&]() { auto __acc = decltype(nums)::value_type{}; for (auto& __item : nums) { __acc = ([](auto acc, auto x) { return (acc + x); })(__acc, __item); } return __acc; })();
    print(r1);
    print(doubled);
    print(sum);
    return 0;
}
