#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <functional>

void print(const std::string& s) {
    std::cout << s << std::endl;
}

void print(int n) {
    std::cout << n << std::endl;
}

void print(double n) {
    std::cout << n << std::endl;
}

int main() {
    print("Hello, World!");
    return 0;
}

