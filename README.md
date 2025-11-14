# LPP ‚Äî Light Programming Language

<div align="center">

**A statically-typed systems language with modern syntax that transpiles to C++**

[Documentation](docs/) ‚Ä¢ [Quick Start](docs/QUICKSTART.md) ‚Ä¢ [Language Spec](docs/FULL_SPEC.md) ‚Ä¢ [Examples](examples/)

</div>

---

## ‚ú® Features

- Ì¥í **Memory Safety** ‚Äî Clang-inspired static analysis with CFG and data-flow tracking
- ‚ö° **High Performance** ‚Äî Transpiles to optimized C++
- ÌæØ **Modern Syntax** ‚Äî Clean, expressive, Rust/JS-inspired
- Ìª°Ô∏è **Static Analysis** ‚Äî Path-sensitive analysis detects bugs before compilation
- Ì¥ß **VS Code Integration** ‚Äî Syntax highlighting + real-time errors
- Ì≥¶ **ES6+ Constructs** ‚Äî Arrow functions, destructuring, spread, and more

## Ì≥Å Project Structure

```
lpp/
‚îú‚îÄ‚îÄ src/           # Compiler source code
‚îú‚îÄ‚îÄ include/       # Header files
‚îú‚îÄ‚îÄ stdlib/        # Minimal standard library
‚îú‚îÄ‚îÄ examples/      # Sample LPP programs
‚îú‚îÄ‚îÄ tests/         # Test suite
‚îú‚îÄ‚îÄ docs/          # Documentation
‚îî‚îÄ‚îÄ README.md
```

## Ì∫Ä Quick Start

### Installation

Download a release or build from source:

```bash
git clone https://github.com/alb0084/lpp.git
cd lpp
cmake -B build
cmake --build build --config Release
```

Compiler output:
- **Windows** ‚Üí `build/Release/lppc.exe`
- **Unix** ‚Üí `build/lppc`

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

## Ì≥ñ Language Overview

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
let [a, b, ...rest] = array;    // destructuring
let {x, y} = point;

let combined = [...a1, ...a2];  // spread
let merged = {...o1, ...o2};

let city = user?.address?.city;       // optional chaining
let username = user?.name ?? "Anon";  // null coalescing
```

## Ìª°Ô∏è Static Analysis

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

- ‚úÖ Division by zero detection
- ‚úÖ Uninitialized variable use
- ‚úÖ Dead code detection
- ‚úÖ Null dereference checks
- ‚úÖ Memory leak detection
- ‚úÖ Integer overflow warnings

## ÌøóÔ∏è Architecture

LPP uses a multi-stage pipeline:

```
Source Code (.lpp)
    ‚Üì
Lexer (Tokenization)
    ‚Üì
Parser (AST Construction)
    ‚Üì
Static Analyzer (CFG + Data-Flow)
    ‚Üì
C++ Transpiler
    ‚Üì
g++/clang (Native Compilation)
    ‚Üì
Executable
```

Details in [ARCHITECTURE.md](docs/ARCHITECTURE.md).

## Ì≥ö Documentation

- **[Quick Start](docs/QUICKSTART.md)**
- **[Language Specification](docs/FULL_SPEC.md)**
- **[Architecture Guide](docs/ARCHITECTURE.md)**
- **[Static Analyzer Details](docs/STATIC_ANALYZER.md)**
- **[ES6+ Features](docs/ES6_FEATURES.md)**
- **[Roadmap](docs/ROADMAP.md)**

## ÌæØ Use Cases

- **Systems Programming** ‚Äî Low-level performance, modern syntax
- **Learning** ‚Äî Great introduction to compilers and type systems
- **Rapid Prototyping** ‚Äî Fast C++ generation without memory headaches
- **Embedded Development** ‚Äî Efficient binaries for constrained devices

## Ì¥ù Contributing

1. Fork repository
2. Create feature branch
3. Commit changes
4. Push branch
5. Open Pull Request

## Ì≥ù Examples

See the [examples/](examples/) directory:

- [hello.lpp](examples/hello.lpp)
- [factorial.lpp](examples/factorial.lpp)
- [advanced_features.lpp](examples/advanced_features.lpp)
- [test_complete_es6.lpp](examples/test_complete_es6.lpp)

## Ì¥ó Related Projects

- **[LightJS](https://github.com/alb0084/lightjs)** ‚Äî The original runtime that inspired LPP

## Ì≥Ñ License

MIT License ‚Äî see [LICENSE](LICENSE).

## Ìπè Acknowledgments

- **Clang Static Analyzer** for CFG-based analysis approach
- **JavaScript/TypeScript** for syntax inspiration
- **Rust** for modern language design principles

---

<div align="center">

**Built with ‚ù§Ô∏è for modern systems programming**

[‚≠ê Star on GitHub](https://github.com/alb0084/lpp) ‚Ä¢ [Ì∞õ Report Bug](https://github.com/alb0084/lpp/issues) ‚Ä¢ [Ì≤° Request Feature](https://github.com/alb0084/lpp/issues)

</div>
