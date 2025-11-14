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

int add5(int x) {
    return (x + 5);
}
int mul2(int x) {
    return (x * 2);
}
int sub3(int x) {
    return (x - 3);
}
int main() {
    auto f1 = []( auto __x ) { return mul2(add5(__x)); };
    auto r1 = f1(10);
    print("mul2 . add5 applied to 10:");
    print(r1);
    auto f2 = []( auto __x ) { return sub3(mul2(add5(__x))); };
    auto r2 = f2(10);
    print("sub3 . mul2 . add5 applied to 10:");
    print(r2);
    return 0;
}
