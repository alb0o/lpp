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

class Person {
public:
    std::string name;
    int age;
    int id;
    
    void greet() {
        print("Hello from Person class!");
    }
    
    std::string getInfo() {
        return "Person info";
    }
    
};

class Counter {
public:
    int count;
    
    void increment() {
        print("Incrementing counter");
    }
    
    int getValue() {
        return 42;
    }
    
};

void main() {
    print("Class properties test completed!");
}
