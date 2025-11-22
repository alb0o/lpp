# L++ v0.8.18 - Dynamic Operator Precedence System

**Release Date:** November 22, 2025  
**Type:** Feature Release  
**Status:** Stable

## 🎯 What's New in v0.8.18

### ✨ Major Feature: Dynamic Operator Precedence System

Implemented a complete **3-tier operator precedence system** inspired by Haskell and Agda, allowing flexible precedence management:

#### **Tier 0: Math Mode (Default)**
- Standard mathematical precedence (14 levels, 0-100 scale)
- Works automatically - no configuration needed
- `2 + 3 * 4` → `(2 + (3 * 4))` = 14 ✅
- `10 - 5 - 2` → `((10 - 5) - 2)` = 3 ✅

#### **Tier 1: Notation Blocks**
```lpp
// Linear mode (left-to-right evaluation)
notation linear {
    const x = 2 + 3 * 4  // 20 (left-to-right)
    const y = 8 / 2 / 2  // 2 (left-to-right)
}

// Custom fixity declarations
notation CustomMath {
    infixl 85 **   // Left-associative power
    infixr 60 |>   // Right-associative pipeline
}
```

#### **Tier 2: Inline Linear Mode**
```lpp
const x = linear(2 + 3 * 4)  // 20 (left-to-right evaluation)
```

### 🏗️ Technical Implementation

**New Components:**
- `PrecedenceTable.h/cpp` - Core precedence management (471 lines)
- `NotationContext` - Stack-based notation scopes
- **Pratt Parser** - Operator precedence climbing algorithm
- Dual-mode parsing: recursive descent (math) + Pratt parser (linear/custom)

**New Syntax:**
- `notation math {}` - Explicit math mode
- `notation linear {}` - Left-to-right evaluation
- `notation CustomName {}` - User-defined precedence scope
- `linear(expr)` - Inline linear evaluation
- `infixl N op` - Left-associative operator at precedence N
- `infixr N op` - Right-associative operator at precedence N
- `infix N op` - Non-associative operator at precedence N

**Token Additions:**
- `NOTATION` - Notation block keyword
- `INFIXL` - Left-associative fixity
- `INFIXR` - Right-associative fixity  
- `INFIX` - Non-associative fixity
- `LINEAR` - Linear mode keyword

### 🐛 Bug Fixes

**Critical:**
- Fixed `const` keyword not recognized in `statement()` - was causing infinite loops
- Fixed `linearExpression()` recursion loop with Pratt parser
- Fixed pipeline operator parsing to maintain multi-stage semantics

**Parser:**
- Added `const` support in variable declarations
- Fixed notation context stack management
- Improved error recovery in precedence parsing

### 📊 Precedence Levels (14 levels)

| Level | Operators | Associativity | Description |
|-------|-----------|---------------|-------------|
| 90 | `.` (composition) | right | Function composition |
| 80 | `**` | right | Exponentiation |
| 70 | `*`, `/`, `%` | left | Multiplicative |
| 60 | `+`, `-` | left | Additive |
| 55 | `..` | left | Range |
| 50 | `<`, `>`, `<=`, `>=` | left | Comparison |
| 45 | `==`, `!=` | left | Equality |
| 40 | `&`, `&&` | left | Logical AND |
| 35 | `|`, `||` | left | Logical OR |
| 30 | `??` | left | Nullish coalescing |
| 10 | `|>` | left | Pipeline |
| 5 | `=` | right | Assignment |

### 🎓 Use Cases

**For Regular Developers (99% of code):**
- Math mode works automatically
- No configuration needed
- Standard precedence "just works"

**For Data Scientists (R/MATLAB porting):**
```lpp
// Pipe operator for data pipelines
const result = data 
    |> filter(age > 18)
    |> map(x => x * 2)
    |> reduce(sum)
```

**For Library Authors (DSL creation):**
```lpp
notation LinearAlgebra {
    infixl 80 *   // Matrix multiplication
    infixl 75 +   // Matrix addition
    // Custom precedence for mathematical notation
}
```

**For Scientific Computing:**
```lpp
// Linear mode for left-to-right calculations
notation linear {
    const tax = income - deduction * rate + penalty
    // Evaluates exactly left-to-right
}
```

### 📝 Documentation Updates

- Added comprehensive "Operator Precedence and Notation System" section to FULL_SPEC.md
- Documented all 3 tiers with examples
- Added fixity declaration syntax
- Included library export/import patterns

### 🚀 Performance

- Precedence lookup: O(1) hash map
- Notation context push/pop: O(1) stack operations
- No runtime overhead - all resolved at compile-time
- Math mode: zero overhead (uses existing recursive descent)

### 🔮 Foundation for Enterprise Features

This release lays the groundwork for future **Enterprise Edition** features:
- Syntax Extension Engine (create new operators)
- AI Validator (validate custom precedence)
- DSL Packs (marketplace of notation systems)

*Note: Enterprise features are not part of the open-source release*

---

## 📦 Installation

### Windows (MSVC)
```bash
cd releases/v0.8.18
# Use lppc.exe and lpprepl.exe directly
```

### Build from Source
```bash
git checkout v0.8.18
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

---

## 📚 Examples

### Basic Math Mode (Default)
```lpp
fn main() -> int {
    const a = 2 + 3 * 4;   // 14
    const b = 10 - 5 - 2;  // 3
    print(a);
    print(b);
    return 0;
}
```

### Linear Mode
```lpp
fn main() -> int {
    // Inline linear
    const x = linear(2 + 3 * 4);  // 20
    
    // Block linear
    notation linear {
        const y = 2 + 3 * 4;  // 20
    }
    
    print(x);
    print(y);
    return 0;
}
```

### Custom Precedence
```lpp
notation MyMath {
    infixl 85 **;  // Change power to left-associative
}

fn main() -> int {
    const result = 2 ** 3 ** 2;  // 64 (left-assoc) not 512 (right-assoc)
    print(result);
    return 0;
}
```

---

## 🔧 Technical Details

**Files Changed:**
- `include/PrecedenceTable.h` (NEW, 125 lines)
- `src/PrecedenceTable.cpp` (NEW, 295 lines)
- `include/Parser.h` (modified - added NotationContext)
- `src/Parser.cpp` (modified - Pratt parser + fixity support)
- `include/Token.h` (added 5 new tokens)
- `src/Lexer.cpp` (added notation keywords)
- `CMakeLists.txt` (added PrecedenceTable.cpp)
- `docs/FULL_SPEC.md` (added precedence documentation)

**Test Results:**
- ✅ All precedence levels validated
- ✅ Associativity working correctly
- ✅ Notation scopes isolated properly
- ✅ No performance regression
- ✅ Backward compatible (math mode default)

---

## 🎉 Credits

Designed and implemented based on research into:
- **Haskell** - Fixity declarations (`infixl`, `infixr`)
- **Agda** - Custom precedence with partial orders
- **SML** - Operator precedence parsing
- **Pratt Parser** - Top-down operator precedence

---

## 📄 License

MIT License - See LICENSE file for details

---

## 🔗 Links

- **Repository:** https://github.com/alb0o/lpp
- **Documentation:** [FULL_SPEC.md](../../docs/FULL_SPEC.md)
- **Previous Release:** [v0.8.17](../v0.8.17/README.md)

---

**Note:** Enterprise features documentation is confidential and not included in public releases.
