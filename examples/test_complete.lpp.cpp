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

// ADT: Result
struct Success {
    int _0;
};
struct Failure {
    std::string _0;
};
using Result = std::variant<
    Success,
    Failure
>;


class Shape {
public:
    virtual ~Shape() = default;
    
    virtual double area() = 0;
    virtual double perimeter() = 0;
};

class Rectangle {
public:
    double width;
    double height;
    
    double area() {
        return (width * height);
    }
    
    double perimeter() {
        return (2 * (width + height));
    }
    
};

class Square : public Rectangle {
public:
    double side;
    
    double area() {
        return (side * side);
    }
    
};

std::string describe(int n) {
    return ([&]() { auto __match_0 = n; if (__match_0 == 0) { return "zero"; } else if (__match_0 == 1) { return "one"; } else if (__match_0 == 2) { return "two"; } })();
}
int square(int x) {
    return (x * x);
}
bool isPositive(int x) {
    return (x > 0);
}
int multiply(int a, int b) {
    return (a * b);
}
int main() {
    print("=== LPP Feature Demo ===");
    print("Pattern matching:");
    print(describe(1));
    print("Higher-order functions:");
    auto nums = std::vector<auto>{1, 2, 3, 4, 5};
    auto squared = map(nums, square);
    auto positives = filter(nums, isPositive);
    auto product = reduce(nums, 1, multiply);
    print("Product:");
    print(product);
    print("All tests completed!");
    return 0;
}
