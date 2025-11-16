# L++ — Light Programming Language

<div align="center">

**A statically-typed systems language with modern syntax that transpiles to C++**

**🎉 PRODUCTION-READY v0.1 - 101 Features Complete! 🎉**

[Quick Start](docs/QUICKSTART.md) • [Language Spec](docs/LANGUAGE_SPEC.md) • [Advanced Features](docs/ADVANCED_FEATURES.md) • [Design Patterns](docs/DESIGN_PATTERNS.md) • [Examples](examples/)

</div>

---

## 🏆 **NEW: Automatic Design Pattern Generation**

L++ introduces the revolutionary `autopattern` keyword that intelligently generates complete implementations of all 23 Gang of Four design patterns!

```lpp
// ONE LINE = COMPLETE PATTERN IMPLEMENTATION!
autopattern Singleton ConfigManager;
autopattern Factory ShapeFactory;
autopattern Observer EventBus;
```

**✨ Features:**
- 🎯 **Intelligent Detection** — Keyword-based pattern recognition
- 🚀 **Zero Boilerplate** — Complete pattern in one line
- 📚 **All 23 GoF Patterns** — Creational, Structural, Behavioral
- 🔧 **Smart Defaults** — Pattern-specific methods and properties
- 💡 **Type Safe** — Full C++ type system integration

[**→ See Complete Pattern Catalog**](docs/COMPLETE_PATTERN_CATALOG.md)

---

## ✨ Core Features

### Language Features
- 🎯 **Modern Syntax** — Rust/JS-inspired (arrow functions, destructuring, spread)
- 🎨 **Multi-Paradigm** — HYBRID, FUNCTIONAL, IMPERATIVE, OOP, GOLFED (5 paradigms!)
- 📦 **ES6+ Support** — Optional chaining (`?.`), nullish coalescing (`??`), template literals
- ⚡ **Golf-Style Operators** — Symbolic functional programming (`~`, `@`, `?`, `\`)
- 🔁 **Iterate-While** — Haskell-inspired sequence generation (`!!<`, `!!>`, `!! $`, `~>`)
- 🎭 **Paradigm Enforcement** — Per-file paradigm declaration with validation
- 🔧 **Pattern Matching** — `match` expressions with guards
- 🧩 **ADTs** — Algebraic data types and type unions
- 🎨 **Higher-Order Functions** — Map, filter, compose, pipeline operator
- 🔗 **Interfaces & Traits** — Protocol-oriented programming
- 📝 **List Comprehensions** — Python-style syntax
- 🌊 **Lambda Expressions** — Closures with capture
- 🎁 **Generators** — `yield` keyword for lazy evaluation
- 🔍 **Type Guards** — `typeof`, `instanceof` operators
- 📊 **Getters/Setters** — Property accessors with `get`/`set`
- ⬅️ **Arrow-Left Returns** — Alternative return syntax (`<-`)

### Static Analysis
- 🛡️ **Path-Sensitive Analysis** — CFG + data-flow tracking
- ✅ **Paradigm Validation** — Enforce functional purity, OOP, or imperative style
- 🔍 **Division by Zero** — Compile-time detection
- ⚠️ **Uninitialized Variables** — Catch bugs before runtime
- 💀 **Dead Code Detection** — Find unreachable code
- 🚫 **Null Dereference** — Safety checks
- 💧 **Memory Leak Detection** — Track allocations
- 🔢 **Integer Overflow** — Warnings for potential overflows

### Developer Experience
- 🔧 **VS Code Extension** — Syntax highlighting + real-time errors
- 📖 **Problem Matcher** — Errors shown directly in editor
- 🚀 **Fast Compilation** — Transpiles to C++ then native code
- ⚡ **High Performance** — Optimized C++ output

---

## 📚 Documentation

### Quick Links
- **[Quick Start Guide](docs/QUICKSTART.md)** - Get started in 5 minutes
- **[Language Specification](docs/LANGUAGE_SPEC.md)** - Complete language reference
- **[Advanced Features](docs/ADVANCED_FEATURES.md)** - ES6+, golf operators, generators, quantum
- **[Design Patterns](docs/DESIGN_PATTERNS.md)** - 23 GoF patterns with autopattern
- **[Paradigms Guide](docs/PARADIGMS.md)** - Multi-paradigm programming
- **[Changelog](CHANGELOG.md)** - Version history and bug fixes

### Full Documentation Index
See **[docs/README.md](docs/README.md)** for complete documentation with topic index.

---

## 🗂️ Project Structure

```
lpp/
├── src/                    # Compiler source code
├── include/                # Header files
├── stdlib/                 # Standard library
├── examples/               # Sample programs
├── tests/                  # Test suite
├── docs/                   # 📚 Complete documentation
│   ├── QUICKSTART.md       # Getting started
│   ├── LANGUAGE_SPEC.md    # Language reference
│   ├── ADVANCED_FEATURES.md # Modern features
│   ├── DESIGN_PATTERNS.md  # Pattern catalog
│   ├── PARADIGMS.md        # Programming styles
│   ├── BUG_FIXES.md        # Bug fix history
│   └── README.md           # Documentation index
├── vscode-extension/       # VS Code integration
├── CHANGELOG.md            # Version history
└── README.md               # This file
```

---

## ��� Quick Start

### Installation

Download a release or build from source:

```bash
git clone https://github.com/alb0084/lpp.git
cd lpp
cmake -B build
cmake --build build --config Release
```

Compiler output:
- **Windows** → `build/Release/lppc.exe`
- **Unix** → `build/lppc`

### Your First Program

Create `hello.lpp`:

```lpp
fn main() -> int {
    println("Hello, LPP!");
    return 0;
}
```

Compile & run:

```bash
lppc hello.lpp
./hello
```

## ��� Language Overview

### Variables & Types

```lpp
let x = 42;           // immutable by default
let mut counter = 0;  // mutable
counter = counter + 1;

let name: string = "LPP";   // explicit types
let pi: float = 3.14159;
```

### Functions

```lpp
fn add(a: int, b: int) -> int {
    return a + b;
}

let multiply = (x, y) => x * y;   // arrow functions
let squares = numbers.map(|n| n*n);
```

### Control Flow

```lpp
if (x > 0) {
    println("positive");
} else if (x < 0) {
    println("negative");
} else {
    println("zero");
}

let result = x > 0 ? "positive" : "negative";  // ternary
```

### Modern Features

```lpp
// Golf-style operators (compact functional programming)
let range = 0~10;                     // [0,1,2,...,10]
let doubled = nums @ (x -> x * 2);    // map
let evens = nums ? (x -> x % 2 == 0); // filter
let sum = nums \ ((acc,x) -> acc+x);  // reduce

// Iterate-while (Haskell-inspired sequence generation)
let countdown = 10 !!> 0;                      // [10,9,8,7,6,5,4,3,2,1]
let powers = 1 !! (x -> x < 100) $ (x -> x*2); // [1,2,4,8,16,32,64]
let squares = 1 ~> (x -> x+1) !! (x -> x < 10) @ (x -> x*x); // [1,4,9,16,25,36,49,64,81]

// Destructuring & spread
let [a, b, ...rest] = array;
let {x, y} = point;
let combined = [...a1, ...a2];

// Optional chaining & nullish coalescing
let city = user?.address?.city;
let username = user?.name ?? "Anon";
```

## ���️ Static Analysis

LPP includes a built-in analyzer that catches issues before compilation:

### Example

```lpp
fn example() -> int {
    let x;
    let y = x + 10;    // ERROR: uninitialized variable

    let z = 10 / 0;    // ERROR: division by zero

    return 0;
    println("unreachable"); // WARNING: dead code
}
```

### Analyzer Capabilities

- ✅ Division by zero detection
- ✅ Uninitialized variable use
- ✅ Dead code detection
- ✅ Null dereference checks
- ✅ Memory leak detection
- ✅ Integer overflow warnings

## ���️ Architecture

LPP uses a multi-stage pipeline:

```
Source Code (.lpp)
    ↓
Lexer (Tokenization)
    ↓
Parser (AST Construction)
    ↓
Static Analyzer (CFG + Data-Flow)
    ↓
C++ Transpiler
    ↓
g++/clang (Native Compilation)
    ↓
Executable
```

Details in [ARCHITECTURE.md](docs/ARCHITECTURE.md).

## 📚 Documentation

- **[Quick Start](docs/QUICKSTART.md)**
- **[Language Specification](docs/FULL_SPEC.md)**
- **[Golf-Style Operators](docs/GOLF_SYNTAX.md)** — Symbolic functional programming
- **[Iterate-While Operators](docs/ITERATE_WHILE.md)** — Haskell-inspired sequence generation
- **[Architecture Guide](docs/ARCHITECTURE.md)**
- **[Static Analyzer Details](docs/STATIC_ANALYZER.md)**
- **[ES6+ Features](docs/ES6_FEATURES.md)**
- **[Roadmap](docs/ROADMAP.md)**

## ��� Use Cases

- **Systems Programming** — Low-level performance, modern syntax
- **Learning** — Great introduction to compilers and type systems
- **Rapid Prototyping** — Fast C++ generation without memory headaches
- **Embedded Development** — Efficient binaries for constrained devices

## ��� Contributing

1. Fork repository
2. Create feature branch
3. Commit changes
4. Push branch
5. Open Pull Request

## ��� Examples

See the [examples/](examples/) directory:

- [hello.lpp](examples/hello.lpp)
- [factorial.lpp](examples/factorial.lpp)
- [advanced_features.lpp](examples/advanced_features.lpp)
- [test_complete_es6.lpp](examples/test_complete_es6.lpp)

## ��� Related Projects

- **[LightJS](https://github.com/alb0084/lightjs)** — The original runtime that inspired LPP

## ��� License

MIT License — see [LICENSE](LICENSE).

## ��� Acknowledgments

- **Clang Static Analyzer** for CFG-based analysis approach
- **JavaScript/TypeScript** for syntax inspiration
- **Rust** for modern language design principles

---

<div align="center">

**Built with ❤️ for modern systems programming**

[⭐ Star on GitHub](https://github.com/alb0084/lpp) • [��� Report Bug](https://github.com/alb0084/lpp/issues) • [��� Request Feature](https://github.com/alb0084/lpp/issues)

</div>
