# LPP Development Roadmap

## ✅ Fase 1: COMPLETATA - Foundation
- ✅ Struttura progetto
- ✅ Specifica linguaggio completa (JS + sintassi simbolica)
- ✅ Token system esteso (150+ token types)
- ✅ Lexer base

## 🔄 Fase 2: IN CORSO - Core Implementation

### Parser (Alta priorità)
- [ ] Estendere AST con nuovi nodi:
  - `TernaryIfExpr` (?->$)
  - `PipelineExpr` (|>)
  - `CompositionExpr` (.)
  - `LambdaExpr`
  - `ListComprehension`
  - `RangeExpr` (..)
  - `SpreadExpr` (...)
  - `DestructurePattern`
  - `TemplateStringExpr`
  - `OptionalChainExpr` (?.)
  - `MatchExpr`
  - `ClassDecl`
  - `InterfaceDecl`
  - `AsyncFn`

- [ ] Implementare parsing per:
  - If ternario simbolico
  - Pipeline operator
  - Function composition
  - Lambda expressions
  - List comprehensions
  - Range syntax
  - Spread/rest operators
  - Destructuring
  - Template strings
  - Optional chaining
  - Match expressions
  - Classes & interfaces
  - Async/await

### Transpiler (Alta priorità)
- [ ] Generazione C++ per:
  - Ternary if → C++ ternary
  - Pipeline → function calls chainati
  - Composition → lambda wrapper
  - Lambda → C++ lambda
  - List comprehension → loops + push_back
  - Range → for loop
  - Spread → vector operations
  - Destructuring → multiple assignments
  - Template strings → string concat
  - Optional chaining → if checks
  - Classes → C++ classes
  - Async → std::future / coroutines

- [ ] Helper runtime library C++:
  - `lpp_runtime.hpp` con funzioni helper
  - Array/vector utilities
  - String utilities
  - Optional/Maybe type
  - Promise wrapper

## ⏳ Fase 3: PIANIFICATA - Advanced Features

### Type System
- [ ] Type inference completo
- [ ] Union types
- [ ] Generic types
- [ ] Type checking statico
- [ ] Type annotations opzionali

### Standard Library
- [ ] Collections (Array, Map, Set)
- [ ] String utilities
- [ ] Math functions
- [ ] File I/O
- [ ] HTTP client
- [ ] JSON parser
- [ ] Regular expressions

### Optimization
- [ ] Dead code elimination
- [ ] Constant folding
- [ ] Inline functions
- [ ] Tail call optimization
- [ ] Pattern matching optimization

## ⏳ Fase 4: PIANIFICATA - Tooling

### CLI Enhancement
- [ ] Watch mode
- [ ] REPL interattivo
- [ ] Syntax highlighting
- [ ] Error messages migliorati
- [ ] Source maps
- [ ] Debugger integration

### Conversion Tools
- [ ] `--convert=symbolic` (JS → simbolico)
- [ ] `--convert=js` (simbolico → JS)
- [ ] Formatter/prettier
- [ ] Linter

### Documentation
- [ ] Language tutorial completo
- [ ] API reference
- [ ] Cookbook con esempi
- [ ] Migration guide da JS
- [ ] Best practices

## ⏳ Fase 5: PIANIFICATA - Ecosystem

### Package Manager
- [ ] `lpp install` per dipendenze
- [ ] Registry packages
- [ ] Semantic versioning
- [ ] Lock file

### IDE Support
- [ ] VS Code extension
  - Syntax highlighting
  - IntelliSense
  - Linting
  - Debugging
  - Refactoring
- [ ] Language Server Protocol (LSP)

### Testing
- [ ] Unit testing framework
- [ ] Assertion library
- [ ] Mocking utilities
- [ ] Coverage tool

### Build System
- [ ] Build profiles (dev, prod)
- [ ] Minification
- [ ] Bundling
- [ ] Tree shaking

## 📊 Progress Tracking

### Current Status: 25% Complete

**Completed:**
- Foundation (100%)
- Token system (100%)
- Basic Lexer (100%)
- Language spec (100%)
- Examples (100%)

**In Progress:**
- Parser (10%)
- Transpiler (10%)
- AST (20%)

**Not Started:**
- Type system (0%)
- Standard library (5% - solo print())
- Optimization (0%)
- Tooling (0%)
- Ecosystem (0%)

## 🎯 Milestones

### Milestone 1: MVP (Target: 2-3 settimane)
**Goal**: Compilare ed eseguire esempi base
- [x] Lexer completo
- [ ] Parser per sintassi base (let, fn, if, while, expressions)
- [ ] Transpiler base per C++
- [ ] CLI funzionante
- [ ] Esempi hello.lpp e factorial.lpp funzionanti

### Milestone 2: Symbolic Syntax (Target: 4-6 settimane)
**Goal**: Supportare sintassi duale completa
- [ ] Parser per ?->$ ternary
- [ ] Parser per pipeline |>
- [ ] Parser per composition .
- [ ] Parser per lambda x->
- [ ] Parser per list comprehension
- [ ] Esempi symbolic_syntax.lpp funzionanti

### Milestone 3: Advanced Features (Target: 8-10 settimane)
**Goal**: Feature JavaScript complete
- [ ] Classes & OOP
- [ ] Async/await
- [ ] Destructuring
- [ ] Spread/rest
- [ ] Template strings
- [ ] Optional chaining
- [ ] Match expressions
- [ ] Esempi advanced_features.lpp funzionanti

### Milestone 4: Production Ready (Target: 12-16 settimane)
**Goal**: Linguaggio stabile e documentato
- [ ] Type system completo
- [ ] Standard library estesa
- [ ] Error handling robusto
- [ ] Performance optimization
- [ ] Documentation completa
- [ ] Test suite completa (>80% coverage)

### Milestone 5: Ecosystem (Target: 16-20 settimane)
**Goal**: Tooling e community
- [ ] VS Code extension
- [ ] Package manager
- [ ] Tutorial e cookbook
- [ ] Website e playground online
- [ ] Community forum

## 🚀 Quick Wins (Facili da implementare)

1. **Range operator (..)** - Semplice sugar syntax per loops
2. **Lambda básico** - JavaScript già lo supporta bene
3. **Ternary if (?->$)** - Mapping diretto a C++ ternary
4. **Template strings** - String concatenation
5. **Spread arrays** - Vector operations

## 🔥 High Impact (Massimo valore)

1. **Pipeline operator (|>)** - Game changer per leggibilità
2. **List comprehension** - Syntax elegante per transformazioni
3. **Pattern matching** - Feature killer per data processing
4. **Type inference** - Developer experience++
5. **VS Code extension** - Adozione facilitata

## 📝 Notes

**Filosofia di sviluppo:**
- Test-driven: ogni feature ha test
- Incremental: feature piccole e complete
- Pragmatic: priorità a usabilità vs purezza
- Performance: sempre profila prima di ottimizzare

**Rischi:**
- Complessità del parser per sintassi duale
- Performance del transpiler per list comprehension
- Compatibilità C++ standard (C++17 vs C++20)
- Manutenzione long-term

**Opportunità:**
- Niche: matematici che programmano
- Education: insegnare FP concepts
- Research: sperimentare syntax design
- Performance: JS syntax con C++ speed
