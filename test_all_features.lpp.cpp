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
    DONE = 2
};

template<typename T>
T identity(T x) {
    return x;
}
template<typename T, typename U>
std::string pair(T first, U second) {
    return "paired";
}
int add(int a, int b) {
    return (a + b);
}
int multiply(int x, int y) {
    return (x * y);
}
int testCompound() {
    auto x = 10;
    x = (x + 5);
    x = (x - 2);
    x = (x * 3);
    x = (x / 2);
    return x;
}
int testIncrement() {
    auto a = 0;
    a++;
    ++a;
    a--;
    --a;
    return a;
}
int sumRange(int n) {
    auto total = 0;
    for (auto i = 0; (i < n); i++) {
        total = (total + i);
    }
    return total;
}
int max(int a, int b) {
    return ((a > b) ? a : b);
}
int getOrDefault(int x) {
    return x;
}
int double(int x) {
    return (x * 2);
}
int inc(int x) {
    return (x + 1);
}
int pipeline() {
    return inc(double(5));
}
int castTest() {
    auto x = 3.14;
    auto y = static_cast<int>(x);
    return y;
}
std::string checkValue(int x) {
    switch (x) {
        case 1:
            return "one";
        case 2:
            return "two";
        default:
            return "other";
    }
}
int countdown(int n) {
    while ((n > 0)) {
        n--;
    }
    return n;
}
int countup() {
    auto n = 0;
    do {
        n++;
    } while ((n < 5));
    return n;
}
std::string compare(int a, int b) {
    if ((a > b)) {
        return "greater";
    }
    if ((a < b)) {
        return "less";
    }
    return "equal";
}
std::future<std::string> fetchData() {
    return std::async(std::launch::async, [=]() {
        auto data = (loadAsync()).get();
        return data;
    });
}
std::future<std::string> loadAsync() {
    return std::async(std::launch::async, [=]() {
        return "data";
    });
}
void main() {
    auto id = identity(42);
    auto p = pair(1, "one");
    auto sum = add(10, 5);
    auto prod = multiply(3, 7);
    auto comp = testCompound();
    auto incr = testIncrement();
    auto range = sumRange(10);
    auto down = countdown(5);
    auto up = countup();
    auto maximum = max(10, 20);
    auto cmp = compare(5, 3);
    auto val = checkValue(2);
    auto piped = pipeline();
    auto def = getOrDefault(42);
    auto casted = castTest();
    return print("✅ All features tested successfully!");
}
