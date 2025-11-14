# LPP - Modern Systems Programming Language# LPP - Light Programming Language



<div align="center">Un linguaggio di programmazione minimalista che transpila in C++.



**A statically-typed systems language with modern syntax that transpiles to C++**## Struttura del Progetto



[Documentation](docs/) • [Quick Start](docs/QUICKSTART.md) • [Language Spec](docs/FULL_SPEC.md) • [Examples](examples/)```

lpp/

</div>├── src/           # Sorgenti del compilatore

├── include/       # Header files

---├── examples/      # Programmi di esempio in LPP

├── tests/         # Test suite

## ✨ Features├── docs/          # Documentazione

├── stdlib/        # Libreria standard minimale

- 🔒 **Memory Safe** - Rust-inspired ownership model with borrow checking└── README.md

- ⚡ **Performance** - Compiles to optimized C++ code```

- 🎯 **Modern Syntax** - Clean, expressive syntax inspired by Rust and JavaScript

- 🛡️ **Static Analysis** - Catch bugs before compilation with Clang-style analyzer## Fasi di Sviluppo

- 🔧 **VS Code Integration** - Full IDE support with syntax highlighting and error detection

- 📦 **ES6+ Features** - Arrow functions, destructuring, spread operators, and more1. ✅ Definizione del core del linguaggio

2. ⏳ Specifica della sintassi (EBNF minima)

## 🚀 Quick Start3. ⏳ Progettazione del lexer (token)

4. ⏳ Progettazione del parser (AST)

### Installation5. ⏳ Definizione dell'AST ufficiale

6. ⏳ Implementazione parser completo

Download the latest release or build from source:7. ⏳ Implementazione transpiler C++

8. ⏳ Integrazione compilazione (Clang)

```bash9. ⏳ Aggiunta stdlib minimale

git clone https://github.com/yourusername/lpp-lang.git10. ⏳ Costruzione CLI (lppc)

cd lpp-lang

cmake -B build## Build

cmake --build build --config Release

``````bash

# TBD - Da definire con CMake o Makefile

The compiler will be available at `build/Release/lppc.exe` (Windows) or `build/lppc` (Unix).```



### VS Code Extension## Utilizzo



Install the VS Code extension for syntax highlighting and error detection:```bash

lppc input.lpp -o output

```bash```

code --install-extension vscode-extension/lpp-language-0.1.0.vsix

```## Licenza



### Your First ProgramTBD


Create `hello.lpp`:

```lpp
fn main() -> int {
    println("Hello, LPP!");
    return 0;
}
```

Compile and run:

```bash
lppc hello.lpp
./hello
```

## 📖 Language Overview

### Variables and Types

```lpp
// Immutable by default (Rust-style)
let x = 42;

// Mutable variables need 'mut' keyword
let mut counter = 0;
counter = counter + 1;

// Type annotations (optional with inference)
let name: string = "LPP";
let pi: float = 3.14159;
```

### Functions

```lpp
// Function with type annotations
fn add(a: int, b: int) -> int {
    return a + b;
}

// Arrow functions (ES6-style)
let multiply = (x, y) => x * y;

// Lambda expressions
let squares = numbers.map(|x| x * x);
```

### Control Flow

```lpp
// If-else
if (x > 0) {
    println("positive");
} else if (x < 0) {
    println("negative");
} else {
    println("zero");
}

// Ternary operator
let result = x > 0 ? "positive" : "negative";

// Pattern matching (planned)
match value {
    0 => "zero",
    1..10 => "small",
    _ => "large"
}
```

### Modern Features

```lpp
// Destructuring
let [first, second, ...rest] = array;
let {x, y} = point;

// Spread operator
let combined = [...array1, ...array2];
let merged = {...obj1, ...obj2};

// Optional chaining
let city = user?.address?.city;

// Null coalescing
let name = user?.name ?? "Unknown";
```

## 🛡️ Static Analysis

LPP includes a powerful static analyzer that catches bugs before compilation:

```lpp
fn example() -> int {
    let x;
    let y = x + 10;  // ERROR: Use of uninitialized variable 'x'
    
    let z = 10 / 0;  // ERROR: Division by zero detected
    
    return 0;
    println("unreachable");  // WARNING: Dead code detected
}
```

### Analysis Features

- ✅ Division by zero detection
- ✅ Uninitialized variable reads
- ✅ Dead code detection
- ✅ Null pointer dereference
- ✅ Memory leak detection
- ✅ Integer overflow warnings

## 🏗️ Architecture

LPP uses a multi-stage compilation pipeline:

```
Source Code (.lpp)
    ↓
Lexer (Tokenization)
    ↓
Parser (AST Construction)
    ↓
Static Analyzer (Bug Detection)
    ↓
Transpiler (C++ Generation)
    ↓
g++/clang (Native Compilation)
    ↓
Executable
```

See [ARCHITECTURE.md](docs/ARCHITECTURE.md) for detailed information.

## 📚 Documentation

- **[Quick Start Guide](docs/QUICKSTART.md)** - Get started in 5 minutes
- **[Language Specification](docs/FULL_SPEC.md)** - Complete language reference
- **[Architecture Guide](docs/ARCHITECTURE.md)** - Compiler internals
- **[Static Analyzer](docs/STATIC_ANALYZER.md)** - Analysis engine details
- **[ES6+ Features](docs/ES6_FEATURES.md)** - Modern JavaScript features
- **[Roadmap](docs/ROADMAP.md)** - Future plans

## 🎯 Use Cases

- **Systems Programming** - Low-level performance with high-level syntax
- **Learning** - Great for understanding compilers and type systems
- **Rapid Prototyping** - Fast C++ code generation without manual memory management
- **Embedded Systems** - Compile to efficient C++ for resource-constrained devices

## 🤝 Contributing

Contributions are welcome! Please read our contributing guidelines before submitting PRs.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📝 Examples

Check out the [examples/](examples/) directory for more:

- [Hello World](examples/hello.lpp)
- [Factorial](examples/factorial.lpp)
- [Advanced Features](examples/advanced_features.lpp)
- [ES6+ Syntax](examples/test_complete_es6.lpp)

## 🔗 Related Projects

- **[LightJS](https://github.com/yourusername/lightjs)** - Modern JavaScript framework (LPP's original use case)

## 📄 License

MIT License - see [LICENSE](LICENSE) for details.

## 🙏 Acknowledgments

- Inspired by Rust's ownership model
- Syntax influenced by JavaScript/TypeScript
- Static analysis techniques from Clang

---

<div align="center">

**Built with ❤️ for modern systems programming**

[⭐ Star on GitHub](https://github.com/yourusername/lpp-lang) • [🐛 Report Bug](https://github.com/yourusername/lpp-lang/issues) • [💡 Request Feature](https://github.com/yourusername/lpp-lang/issues)

</div>
