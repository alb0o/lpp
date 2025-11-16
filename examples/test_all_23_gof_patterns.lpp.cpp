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

class ConfigManager {
// AUTO-GENERATED: Singleton Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_SINGLETON(ConfigManager)
public:
    ConfigManager* instance;
    
    ConfigManager* getInstance() {
        return instance;
    }
    
};

class ShapeFactory {
// AUTO-GENERATED: Factory Pattern
#include "lpp_patterns.hpp"
public:
    map<string, Product*> products;
    
    Product* create(std::string type) {
        return;
    }
    
};

class UIComponentFactory {
// AUTO-GENERATED: AbstractFactory Pattern
#include "lpp_patterns.hpp"
public:
    map<string, Factory*> factories;
    
    ProductA* createProductA() {
    }
    
    ProductB* createProductB() {
    }
    
};

class DocumentBuilder {
// AUTO-GENERATED: Builder Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_BUILDER(DocumentBuilder)
public:
    Product* product;
    
    void buildPartA() {
    }
    
    Product* getResult() {
    }
    
};

class ShapePrototype {
// AUTO-GENERATED: Prototype Pattern
#include "lpp_patterns.hpp"
public:
    
    ShapePrototype* clone() {
    }
    
};

class LegacySystemAdapter {
// AUTO-GENERATED: Adapter Pattern
#include "lpp_patterns.hpp"
public:
    Adaptee* adaptee;
    
    void request() {
    }
    
};

class DrawingAPI {
// AUTO-GENERATED: Bridge Pattern
#include "lpp_patterns.hpp"
public:
    Implementor* implementor;
    
    void operation() {
    }
    
};

class FileSystemNode {
// AUTO-GENERATED: Composite Pattern
#include "lpp_patterns.hpp"
public:
    vector<Component*> children;
    
    void add(Component* component) {
    }
    
    void operation() {
    }
    
};

class TextDecorator {
// AUTO-GENERATED: Decorator Pattern
#include "lpp_patterns.hpp"
public:
    Component* component;
    
    void operation() {
    }
    
};

class MultimediaSystemFacade {
// AUTO-GENERATED: Facade Pattern
#include "lpp_patterns.hpp"
public:
    Subsystem1* subsystem1;
    Subsystem2* subsystem2;
    
    void operation() {
    }
    
};

class CharacterFactory {
// AUTO-GENERATED: Flyweight Pattern
#include "lpp_patterns.hpp"
public:
    std::string intrinsicState;
    map<string, Flyweight*> pool;
    
    void operation(std::string extrinsicState) {
    }
    
};

class ImageProxy {
// AUTO-GENERATED: Proxy Pattern
#include "lpp_patterns.hpp"
public:
    RealSubject* realSubject;
    
    void request() {
    }
    
};

class ErrorHandler {
// AUTO-GENERATED: ChainOfResponsibility Pattern
#include "lpp_patterns.hpp"
public:
    Handler* successor;
    
    void handleRequest(Request* request) {
    }
    
};

class RemoteControl {
// AUTO-GENERATED: Command Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_COMMAND()
public:
    Receiver* receiver;
    
    void execute() {
    }
    
    void undo() {
    }
    
};

class ListIterator {
// AUTO-GENERATED: Iterator Pattern
#include "lpp_patterns.hpp"
public:
    Collection* collection;
    int current;
    
    Item* next() {
    }
    
    bool hasNext() {
    }
    
};

class ChatRoom {
// AUTO-GENERATED: Mediator Pattern
#include "lpp_patterns.hpp"
public:
    vector<Colleague*> colleagues;
    
    void notify(Colleague* sender, std::string event) {
    }
    
};

class EditorStateManager {
// AUTO-GENERATED: Memento Pattern
#include "lpp_patterns.hpp"
public:
    std::string state;
    
    Memento* save() {
    }
    
    void restore(Memento* memento) {
    }
    
};

class NewsPublisher {
// AUTO-GENERATED: Observer Pattern
#include "lpp_patterns.hpp"
LPP_PATTERN_OBSERVER(NewsPublisher)
public:
    vector<Observer*> observers;
    
    void attach(Observer* observer) {
    }
    
    void notify() {
    }
    
};

class ConnectionState {
// AUTO-GENERATED: State Pattern
#include "lpp_patterns.hpp"
public:
    State* state;
    
    void handle() {
    }
    
    void setState(State* newState) {
    }
    
};

class SortStrategy {
// AUTO-GENERATED: Strategy Pattern
#include "lpp_patterns.hpp"
public:
    Strategy* strategy;
    
    void execute() {
    }
    
};

class DataProcessor {
// AUTO-GENERATED: TemplateMethod Pattern
#include "lpp_patterns.hpp"
public:
    
    void templateMethod() {
    }
    
    void step1() {
    }
    
    void step2() {
    }
    
};

class ShapeVisitor {
// AUTO-GENERATED: Visitor Pattern
#include "lpp_patterns.hpp"
public:
    
    void visit(Element* element) {
    }
    
};

class ExpressionInterpreter {
// AUTO-GENERATED: Interpreter Pattern
#include "lpp_patterns.hpp"
public:
    Context* context;
    
    void interpret() {
    }
    
};

