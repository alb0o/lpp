# LPP Compiler Architecture

## Overview

LPP is a statically-typed systems programming language that transpiles to C++. It features Rust-like ownership semantics, modern syntax, and comprehensive static analysis.

## Compilation Pipeline

```
Source Code (.lpp)
    ↓
[Lexer] - Tokenization
    ↓
[Parser] - AST Construction
    ↓
[Static Analyzer] - Bug Detection
    ↓
[Transpiler] - C++ Code Generation
    ↓
Generated C++ (.cpp)
    ↓
[g++/clang] - Native Compilation
    ↓
Executable
```

## Directory Structure

```
lpp/
├── include/              # Header files
│   ├── Token.h          # Token types and definitions
│   ├── Lexer.h          # Lexical analyzer
│   ├── Parser.h         # Syntax parser
│   ├── AST.h            # Abstract Syntax Tree nodes
│   ├── Transpiler.h     # C++ code generator
│   └── StaticAnalyzer.h # Static analysis engine
│
├── src/                  # Implementation files
│   ├── main.cpp         # Entry point
│   ├── Token.cpp
│   ├── Lexer.cpp
│   ├── Parser.cpp
│   ├── AST.cpp
│   ├── Transpiler.cpp
│   └── StaticAnalyzer.cpp
│
├── tests/               # Test LPP programs
├── examples/            # Example programs
├── stdlib/              # Standard library (future)
├── docs/                # Language specification
└── vscode-extension/    # VS Code integration
```

## Core Components

### 1. Lexer (Lexical Analyzer)

**File:** `src/Lexer.cpp`, `include/Lexer.h`

**Responsibility:** Converts source text into tokens.

**Key Methods:**
- `tokenize()` - Main entry point
- `scanToken()` - Processes individual tokens
- `identifier()` - Handles identifiers and keywords
- `number()` - Parses numeric literals
- `string()` - Parses string literals

**Token Types:**
- Keywords: `fn`, `let`, `mut`, `if`, `while`, `return`, etc.
- Operators: `+`, `-`, `*`, `/`, `==`, `!=`, etc.
- Literals: numbers, strings, booleans
- Symbols: `{`, `}`, `(`, `)`, `,`, `;`, etc.

### 2. Parser

**File:** `src/Parser.cpp`, `include/Parser.h`

**Responsibility:** Builds Abstract Syntax Tree from tokens.

**Key Methods:**
- `parse()` - Entry point, returns Program AST
- `declaration()` - Parses top-level declarations
- `functionDecl()` - Function definitions
- `varDeclaration()` - Variable declarations (with `mut` support)
- `statement()` - Statements (if, while, return, etc.)
- `expression()` - Expression parsing (precedence climbing)

**Grammar (simplified):**
```
program     → declaration* EOF
declaration → functionDecl | varDeclaration
functionDecl → "fn" IDENTIFIER "(" params ")" "->" type block
statement   → exprStmt | ifStmt | whileStmt | returnStmt | block
expression  → assignment | ternary | binary | unary | primary
```

### 3. AST (Abstract Syntax Tree)

**File:** `src/AST.cpp`, `include/AST.h`

**Responsibility:** Represents program structure.

**Node Types:**
- **Expressions:** NumberExpr, StringExpr, BinaryExpr, CallExpr, etc.
- **Statements:** VarDeclStmt, IfStmt, WhileStmt, ReturnStmt, etc.
- **Declarations:** FunctionDecl, StructDecl
- **Program:** Root node containing all declarations

**Visitor Pattern:** Used for traversing AST (Transpiler, StaticAnalyzer).

### 4. Static Analyzer

**File:** `src/StaticAnalyzer.cpp`, `include/StaticAnalyzer.h`

**Responsibility:** Detects bugs before transpilation using Clang-style analysis.

**Analysis Types:**
- **Control Flow Analysis:** Builds CFG (Control Flow Graph)
- **Data Flow Analysis:** Tracks variable states through symbolic execution
- **Path-sensitive Analysis:** Explores all execution paths

**Issue Detection:**
- Division by zero
- Uninitialized variable reads
- Dead code
- Null pointer dereferences
- Memory leaks
- Integer overflow

**Key Methods:**
- `analyze()` - Main entry point
- `buildCFG()` - Constructs control flow graph
- `runDataFlowAnalysis()` - Worklist algorithm for fixpoint iteration
- `checkDivisionByZero()`, `checkUninitializedRead()`, etc.

**Algorithms:**
- Worklist-based fixpoint iteration
- Conservative state merging at join points
- Symbolic execution with abstract interpretation

### 5. Transpiler

**File:** `src/Transpiler.cpp`, `include/Transpiler.h`

**Responsibility:** Generates C++ code from AST.

**Key Methods:**
- `transpile()` - Entry point, returns C++ string
- `visitXXX()` - Visitor methods for each AST node type
- `generateIncludes()` - Standard C++ headers
- `generateMain()` - Main function wrapper

**Transpilation Rules:**
- `let x = 5` → `auto x = 5;`
- `let mut x = 5` → `int x = 5;`
- `fn foo() -> int` → `int foo()`
- `println(x)` → `std::cout << x << std::endl;`

### 6. Main Driver

**File:** `src/main.cpp`

**Responsibility:** Orchestrates compilation pipeline.

**Flow:**
1. Read source file
2. Lex into tokens
3. Parse into AST
4. Run static analysis (fails on errors)
5. Transpile to C++
6. Write output file
7. Compile with g++/clang (optional)

## Key Design Decisions

### Memory Safety

**Choice:** Rust-like ownership model with borrow checker  
**Status:** Initially implemented, replaced with Clang-style static analyzer  
**Reason:** Better fit for C++ target language

### Static Analysis

**Choice:** Clang-style CFG + symbolic execution  
**Benefits:**
- Catches real bugs (div-by-zero, uninitialized reads)
- Path-sensitive analysis
- Appropriate for C++ output

### Transpilation Target

**Choice:** C++ instead of LLVM IR  
**Benefits:**
- Easier debugging (readable output)
- Leverage C++ compiler optimizations
- Simpler implementation

## Adding New Features

### 1. Add a New Keyword

**Example:** Adding `const` keyword

1. **Token.h:** Add `CONST` to TokenType enum
2. **Lexer.cpp:** Add `{"const", TokenType::CONST}` to keywords map
3. **Parser.cpp:** Handle in appropriate parsing function
4. **AST.h:** Add/modify AST node if needed
5. **Transpiler.cpp:** Generate corresponding C++ code
6. **StaticAnalyzer.cpp:** Update analysis logic if needed

### 2. Add a New Operator

**Example:** Adding `**` (power) operator

1. **Token.h:** Add `POWER` token type
2. **Lexer.cpp:** Scan `**` in `scanToken()`
3. **Parser.cpp:** Add to operator precedence in `binary()`
4. **Transpiler.cpp:** Generate `std::pow(left, right)`

### 3. Add a New Statement

**Example:** Adding `for` loop

1. **AST.h:** Create `ForStmt` class
2. **Parser.cpp:** Add `forStatement()` method
3. **Transpiler.cpp:** Implement `visitForStmt()`
4. **StaticAnalyzer.cpp:** Update CFG construction for loops

### 4. Add Static Analysis Check

**Example:** Detecting buffer overflows

1. **StaticAnalyzer.h:** Add `BUFFER_OVERFLOW` to IssueType
2. **StaticAnalyzer.cpp:** Implement `checkBufferOverflow()`
3. **Call in `analyze()` method

## Build System

### CMake Configuration

**File:** `CMakeLists.txt`

**Targets:**
- `lppc` - Main compiler executable
- Requires C++17 standard

**Building:**
```bash
cmake -B build
cmake --build build --config Release
```

### Output

**Compiler:** `build/Release/lppc.exe` (Windows) or `build/lppc` (Unix)

## Testing

### Test Structure

**Directory:** `tests/`

**Test Files:**
- `test_clean_code.lpp` - Valid code (should pass)
- `test_division_by_zero.lpp` - Error detection test
- `test_uninitialized_var.lpp` - Error detection test
- `test_dead_code.lpp` - Warning detection test

**Running Tests:**
```bash
./build/Release/lppc.exe tests/test_clean_code.lpp
```

## VS Code Integration

### Extension Structure

**Directory:** `vscode-extension/`

**Features:**
- Syntax highlighting (TextMate grammar)
- Problem matcher (parses compiler output)
- Build tasks (Ctrl+Shift+B)
- Code snippets
- Language configuration (brackets, comments)

**Installation:**
```bash
code --install-extension vscode-extension/lpp-language-0.1.0.vsix
```

## Performance Considerations

### Compilation Speed

**Current:** ~50ms for small programs (<500 LOC)

**Bottlenecks:**
- Static analysis (CFG construction)
- C++ compilation (g++/clang)

**Optimizations:**
- Incremental compilation (future)
- Caching of analysis results (future)

### Memory Usage

**Current:** ~50-100 MB for typical programs

**Managed by:**
- Smart pointers (`std::unique_ptr`) for AST nodes
- Efficient token storage
- CFG node reuse

## Future Enhancements

### Language Features
- Generics/Templates
- Pattern matching
- Async/await
- Modules system
- Macros

### Tooling
- Language Server Protocol (LSP)
- Debugger integration
- Package manager
- REPL

### Analysis
- Inter-procedural analysis
- Concurrency bug detection
- Taint tracking for security
- Model checking integration

### Backends
- Direct LLVM IR generation
- WebAssembly target
- Native ARM/x86 codegen

## Resources

- **Language Spec:** `docs/FULL_SPEC.md`
- **Static Analyzer:** `STATIC_ANALYZER.md`
- **ES6+ Features:** `ES6_FEATURES.md`
- **Quick Start:** `QUICKSTART.md`
- **Roadmap:** `ROADMAP.md`

## Contributing

When contributing:
1. Follow existing code style
2. Add tests for new features
3. Update documentation
4. Run static analysis on your changes
5. Ensure all tests pass

## License

[Add license information]
