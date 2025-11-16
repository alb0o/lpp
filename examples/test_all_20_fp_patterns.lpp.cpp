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

class ResultMonad {
// AUTO-GENERATED: Monad Pattern
#include "lpp_patterns.hpp"
public:
    T value;
    
    Monad<U> bind(function<Monad<U>(T)> f) {
    }
    
    Monad<T> pure(T x) {
    }
    
};

class ListFunctor {
// AUTO-GENERATED: Functor Pattern
#include "lpp_patterns.hpp"
public:
    T value;
    
    Functor<U> map(function<U(T)> f) {
    }
    
};

class ValidationApplicative {
// AUTO-GENERATED: Applicative Pattern
#include "lpp_patterns.hpp"
public:
    T value;
    
    Applicative<U> apply(Applicative<function<U(T)>> f) {
    }
    
    Applicative<T> pure(T x) {
    }
    
};

class OptionalValue {
// AUTO-GENERATED: Maybe Pattern
#include "lpp_patterns.hpp"
public:
    optional<T> value;
    
    Maybe<T> just(T x) {
    }
    
    Maybe<T> nothing() {
    }
    
    T getOrElse(T defaultValue) {
    }
    
};

class ErrorResult {
// AUTO-GENERATED: Either Pattern
#include "lpp_patterns.hpp"
public:
    variant<L, R> value;
    bool isLeft;
    
    Either<L, R> left(L l) {
    }
    
    Either<L, R> right(R r) {
    }
    
    T fold(function<T(L)> onLeft, function<T(R)> onRight) {
    }
    
};

class StringMonoid {
// AUTO-GENERATED: Monoid Pattern
#include "lpp_patterns.hpp"
public:
    T value;
    
    Monoid<T> combine(Monoid<T> other) {
    }
    
    Monoid<T> empty() {
    }
    
};

class TreeFoldable {
// AUTO-GENERATED: Foldable Pattern
#include "lpp_patterns.hpp"
public:
    Container<T> container;
    
    U foldLeft(U init, function<U(U, T)> f) {
    }
    
    U foldRight(U init, function<U(T, U)> f) {
    }
    
};

class ListTraversable {
// AUTO-GENERATED: Traversable Pattern
#include "lpp_patterns.hpp"
public:
    Container<T> container;
    
    Applicative<Container<U>> traverse(function<Applicative<U>(T)> f) {
    }
    
    Applicative<Container<T>> sequence() {
    }
    
};

class PersonLens {
// AUTO-GENERATED: Lens Pattern
#include "lpp_patterns.hpp"
public:
    function<T(S)> getter;
    function<S(S, T)> setter;
    
    T view(S s) {
    }
    
    S set(S s, T t) {
    }
    
    S over(S s, function<T(T)> f) {
    }
    
};

class ListZipper {
// AUTO-GENERATED: Zipper Pattern
#include "lpp_patterns.hpp"
public:
    vector<T> left;
    T focus;
    vector<T> right;
    
    Zipper<T> moveLeft() {
    }
    
    Zipper<T> moveRight() {
    }
    
    Zipper<T> update(function<T(T)> f) {
    }
    
};

class GameState {
// AUTO-GENERATED: State Pattern
#include "lpp_patterns.hpp"
public:
    State* state;
    
    void handle() {
    }
    
    void setState(State* newState) {
    }
    
};

class ConfigReader {
// AUTO-GENERATED: Reader Pattern
#include "lpp_patterns.hpp"
public:
    function<A(R)> runReader;
    
    Reader<R, R> ask() {
    }
    
    Reader<R, A> local(function<R(R)> f) {
    }
    
};

class LogWriter {
// AUTO-GENERATED: Writer Pattern
#include "lpp_patterns.hpp"
public:
    A value;
    W log;
    
    Writer<void, W> tell(W msg) {
    }
    
    Writer<pair<A, W>, W> listen() {
    }
    
};

class FileIO {
// AUTO-GENERATED: IO Pattern
#include "lpp_patterns.hpp"
public:
    function<A()> action;
    
    A unsafePerformIO() {
    }
    
    IO<B> bind(function<IO<B>(A)> f) {
    }
    
};

class AsyncContinuation {
// AUTO-GENERATED: Continuation Pattern
#include "lpp_patterns.hpp"
public:
    function<R(function<R(A)>)> runCont;
    
    Cont<R, A> callCC(function<Cont<R, A>(function<Cont<R, B>(A)>)> f) {
    }
    
    Cont<R, B> bind(function<Cont<R, B>(A)> f) {
    }
    
};

class DSLProgram {
// AUTO-GENERATED: Free Pattern
#include "lpp_patterns.hpp"
public:
    variant<Pure<A>, Impure<F, Free<F, A>>> tree;
    
    Free<F, A> pure(A a) {
    }
    
    Free<F, A> impure(F<Free<F, A>> f) {
    }
    
    M<A> interpret(function<M<A>(F<A>)> interpreter) {
    }
    
};

class StreamComonad {
// AUTO-GENERATED: Comonad Pattern
#include "lpp_patterns.hpp"
public:
    W<A> value;
    
    A extract() {
    }
    
    W<B> extend(function<B(W<A>)> f) {
    }
    
    W<W<A>> duplicate() {
    }
    
};

class ChurchList {
// AUTO-GENERATED: Church Pattern
#include "lpp_patterns.hpp"
public:
    function<R(function<R(A)>, function<R()>)> churchEncoding;
    
    R fold(function<R(A)> onValue, function<R()> onEmpty) {
    }
    
    Church<A> encode(A value) {
    }
    
};

class EffectSystem {
// AUTO-GENERATED: IO Pattern
#include "lpp_patterns.hpp"
public:
    function<A()> action;
    
    A unsafePerformIO() {
    }
    
    IO<B> bind(function<IO<B>(A)> f) {
    }
    
};

class RecursiveScheme {
// AUTO-GENERATED: Factory Pattern
#include "lpp_patterns.hpp"
public:
    map<string, Product*> products;
    
    Product* create(std::string type) {
        return;
    }
    
};

