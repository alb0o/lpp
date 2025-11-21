# L++ Documentation v0.8.17

Complete documentation for the L++ programming language.

---

## 📚 Quick Navigation

### 🚀 Start Here
1. **[QUICKSTART.md](QUICKSTART.md)** - Get up and running in 5 minutes
2. **[FULL_SPEC.md](FULL_SPEC.md)** - Complete language specification

### 💡 Features & Patterns
- **[ADVANCED_FEATURES.md](ADVANCED_FEATURES.md)** - Modern features (ES6+, golf, quantum, generators)
- **[MOLECULE_FEATURE.md](MOLECULE_FEATURE.md)** - Molecular programming API
- **[DESIGN_PATTERNS.md](DESIGN_PATTERNS.md)** - All 23 GoF patterns with @autopattern
- **[PARADIGMS.md](PARADIGMS.md)** - Multi-paradigm guide (OOP, FP, Golf, Hybrid)

### 🔧 Architecture & Tools
- **[ARCHITECTURE.md](ARCHITECTURE.md)** - Compiler internals (Lexer→Parser→AST→Transpiler)
- **[STATIC_ANALYZER.md](STATIC_ANALYZER.md)** - Static analysis & CFG system
- **[BUG_FIXES.md](BUG_FIXES.md)** - Bug fix history
- **[ROADMAP.md](ROADMAP.md)** - Future development plans

---

## 📦 Deprecated/Consolidated Files

Historical bug tracking files have been consolidated into **[BUG_FIXES.md](BUG_FIXES.md)** which now contains:
- All 45 bugs fixed in v0.8.17 (Sessions 1-3)
- Security audit results (9.5/10)
- RAII implementation details
- Thread safety improvements
- Historical bug fixes from v0.8.16

---

## 📖 Documentation Index by Topic

### Type System
- Type annotations (`:` syntax)
- Type inference with `auto`
- Nullable types (`int?`)
- Union types (`int | string`)
- Arrays (dynamic and fixed-size)
- Generics (`<T>`)

**Reference:** [FULL_SPEC.md](FULL_SPEC.md#type-system)

---

### Control Flow
- If/else statements
- For/while/do-while loops
- Switch-case with guards
- Try-catch-finally
- Break/continue

**Reference:** [FULL_SPEC.md](FULL_SPEC.md#control-flow)

---

### Functions
- Function declarations
- Arrow functions (`->`, `=>`)
- Rest parameters (`...args`)
- Async/await
- Generators (`yield`)
- Implicit return

**Reference:** [ADVANCED_FEATURES.md](ADVANCED_FEATURES.md#es6-modern-features)

---

### Object-Oriented
- Classes and inheritance
- Interfaces (`implements`)
- Methods and properties
- Getters/setters
- Access modifiers

**Reference:** [PARADIGMS.md](PARADIGMS.md#oop-paradigm)

---

### Functional Programming
- Higher-order functions
- Map/filter/reduce
- Pipeline operator (`|>`)
- List comprehensions
- Immutability

**Reference:** [PARADIGMS.md](PARADIGMS.md#functional-paradigm)

---

### Golf-Style Operators
- Range: `0~10`
- Map: `array @ fn`
- Filter: `array ? pred`
- Reduce: `array \ fn`

**Reference:** [ADVANCED_FEATURES.md](ADVANCED_FEATURES.md#golf-style-operators)

---

### Advanced Features
- Quantum variables (experimental)
- Type guards (`typeof`, `instanceof`)
- Destructuring (array/object)
- Template literals
- Optional chaining (`?.`)
- Nullish coalescing (`??`)
- Spread operator (`...`)

**Reference:** [ADVANCED_FEATURES.md](ADVANCED_FEATURES.md)

---

### Design Patterns
- Autopattern keyword
- 23 Gang of Four patterns:
  - **Creational:** Singleton, Factory, Builder, Prototype, Abstract Factory
  - **Structural:** Adapter, Bridge, Composite, Decorator, Facade, Flyweight, Proxy
  - **Behavioral:** Chain of Responsibility, Command, Interpreter, Iterator, Mediator, Memento, Observer, State, Strategy, Template Method, Visitor

**Reference:** [DESIGN_PATTERNS.md](DESIGN_PATTERNS.md)

---

### Static Analysis
- Dead code detection
- Uninitialized variables
- Division by zero
- Type compatibility
- Control flow graph (CFG)
- Paradigm validation

**Reference:** [STATIC_ANALYZER.md](STATIC_ANALYZER.md)

---

### Multi-Paradigm
- HYBRID (default)
- FUNCTIONAL (pure functions)
- IMPERATIVE (explicit loops)
- OOP (classes)
- GOLFED (ultra-minimal)

**Reference:** [PARADIGMS.md](PARADIGMS.md)

---

## 🔧 Tools & Integration

### VS Code Extension
Located in `vscode-extension/` directory:
- Syntax highlighting
- Problem matcher
- Code snippets
- Language configuration

**Install:** See `vscode-extension/README.md`

---

### Build System
- CMake build configuration
- MSVC support (Windows)
- GCC/Clang support (Linux/macOS)

---

## 📊 Status

**Version:** 0.8.17  
**Status:** ✅ Production-Ready  
**Features:** 101/101 (100% Complete)  
**Bugs Fixed:** 13 critical bugs resolved  

---

## 🤝 Contributing

See main [README.md](../README.md) for contribution guidelines.

---

## 📄 License

L++ is open source. See [LICENSE](../LICENSE) for details.

---

## Quick Reference Card

```lpp
// Variables
let x = 42;
let name: string = "LPP";

// Functions
fn add(a: int, b: int) -> int {
    return a + b;
}

// Arrow functions
let double = x => x * 2;

// Classes
class Point {
    let x: int;
    let y: int;
    
    fn distance() -> float {
        return sqrt(this.x * this.x + this.y * this.y);
    }
}

// Arrays & comprehensions
let nums = [1, 2, 3, 4, 5];
let evens = [x | x in nums, x % 2 == 0];

// Golf operators
let result = 0~100 ? (x => x % 2 == 0) @ (x => x * x) \ ((a, b) => a + b);

// Design patterns
autopattern Singleton Logger;

// Quantum (experimental)
quantum let dice = [1, 2, 3, 4, 5, 6];
let roll = dice.observe();
```

---

**Ready to start?** → [QUICKSTART.md](QUICKSTART.md)
