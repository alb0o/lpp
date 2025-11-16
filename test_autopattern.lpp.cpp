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

class AppConfig {
// AUTO-GENERATED: Singleton Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_SINGLETON(AppConfig)
public:
    AppConfig* instance;
    
    void AppConfig() {
    }
    
    AppConfig* getInstance() {
        return instance;
    }
    
};

class UserFactory {
// AUTO-GENERATED: Factory Pattern
#include "lpp_patterns.hpp"
public:
    map<string, Product*> products;
    
    Product* create(std::string type) {
        return;
    }
    
};

class EventManager {
// AUTO-GENERATED: Observer Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_OBSERVER(EventManager)
public:
    vector<Observer*> observers;
    
    void attach(Observer* observer) {
    }
    
    void notify() {
    }
    
};

void main() {
    return print("Autopattern generates design patterns!");
}
