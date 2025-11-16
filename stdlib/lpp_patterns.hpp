// LPP Design Pattern Generator
// Use @pattern directive in OOP paradigm to auto-generate common patterns

#ifndef LPP_PATTERNS_H
#define LPP_PATTERNS_H

// This header is generated when @pattern keyword is detected in OOP mode

namespace lpp
{
    namespace patterns
    {

// ===== SINGLETON PATTERN =====
// Usage: @pattern Singleton
// Generates thread-safe singleton with lazy initialization
#define LPP_PATTERN_SINGLETON(ClassName)              \
private:                                              \
    static ClassName *instance;                       \
    static std::mutex mutex;                          \
    ClassName() {}                                    \
    ClassName(const ClassName &) = delete;            \
    ClassName &operator=(const ClassName &) = delete; \
                                                      \
public:                                               \
    static ClassName *getInstance()                   \
    {                                                 \
        std::lock_guard<std::mutex> lock(mutex);      \
        if (instance == nullptr)                      \
        {                                             \
            instance = new ClassName();               \
        }                                             \
        return instance;                              \
    }

// ===== FACTORY PATTERN =====
// Usage: @pattern Factory
// Generates factory method for object creation
#define LPP_PATTERN_FACTORY(BaseClass, ConcreteClass) \
public:                                               \
    static BaseClass *create(const std::string &type) \
    {                                                 \
        if (type == #ConcreteClass)                   \
        {                                             \
            return new ConcreteClass();               \
        }                                             \
        return nullptr;                               \
    }

// ===== OBSERVER PATTERN =====
// Usage: @pattern Observer
// Generates observer/subscriber mechanism
#define LPP_PATTERN_OBSERVER(ClassName)            \
private:                                           \
    std::vector<std::function<void()>> observers;  \
                                                   \
public:                                            \
    void subscribe(std::function<void()> callback) \
    {                                              \
        observers.push_back(callback);             \
    }                                              \
    void notify()                                  \
    {                                              \
        for (auto &observer : observers)           \
        {                                          \
            observer();                            \
        }                                          \
    }

// ===== BUILDER PATTERN =====
// Usage: @pattern Builder
// Generates fluent builder interface
#define LPP_PATTERN_BUILDER(ClassName)      \
public:                                     \
    class Builder                           \
    {                                       \
    private:                                \
        ClassName *obj;                     \
                                            \
    public:                                 \
        Builder() : obj(new ClassName()) {} \
        ClassName *build() { return obj; }  \
    };

// ===== STRATEGY PATTERN =====
// Usage: @pattern Strategy
// Generates strategy interface
#define LPP_PATTERN_STRATEGY(StrategyName) \
public:                                    \
    class StrategyName                     \
    {                                      \
    public:                                \
        virtual void execute() = 0;        \
        virtual ~StrategyName() = default; \
    };

// ===== DECORATOR PATTERN =====
// Usage: @pattern Decorator
// Generates decorator wrapper
#define LPP_PATTERN_DECORATOR(BaseClass) \
private:                                 \
    BaseClass *wrapped;                  \
                                         \
public:                                  \
    explicit Decorator(BaseClass *obj) : wrapped(obj) {}

// ===== ADAPTER PATTERN =====
// Usage: @pattern Adapter
// Generates adapter interface
#define LPP_PATTERN_ADAPTER(TargetInterface, Adaptee) \
private:                                              \
    Adaptee *adaptee;                                 \
                                                      \
public:                                               \
    explicit Adapter(Adaptee *obj) : adaptee(obj) {}

// ===== FACADE PATTERN =====
// Usage: @pattern Facade
// Generates simplified interface
#define LPP_PATTERN_FACADE(SubsystemName)        \
private:                                         \
    SubsystemName *subsystem;                    \
                                                 \
public:                                          \
    Facade() : subsystem(new SubsystemName()) {} \
    void simplifiedOperation() { subsystem->complexOperation(); }

// ===== PROXY PATTERN =====
// Usage: @pattern Proxy
// Generates lazy-loading proxy
#define LPP_PATTERN_PROXY(RealSubject)       \
private:                                     \
    RealSubject *realSubject = nullptr;      \
                                             \
public:                                      \
    void request()                           \
    {                                        \
        if (realSubject == nullptr)          \
        {                                    \
            realSubject = new RealSubject(); \
        }                                    \
        realSubject->request();              \
    }

// ===== COMMAND PATTERN =====
// Usage: @pattern Command
// Generates command interface
#define LPP_PATTERN_COMMAND()         \
public:                               \
    class Command                     \
    {                                 \
    public:                           \
        virtual void execute() = 0;   \
        virtual ~Command() = default; \
    };

// ===== CHAIN OF RESPONSIBILITY =====
// Usage: @pattern ChainOfResponsibility
// Generates handler chain
#define LPP_PATTERN_CHAIN()                            \
private:                                               \
    Handler *next = nullptr;                           \
                                                       \
public:                                                \
    void setNext(Handler *handler) { next = handler; } \
    void handleRequest()                               \
    {                                                  \
        if (next != nullptr)                           \
        {                                              \
            next->handleRequest();                     \
        }                                              \
    }

    } // namespace patterns
} // namespace lpp

#endif // LPP_PATTERNS_H
