# Advanced Features - L++ Language v0.8.17

Comprehensive guide to L++'s modern and experimental language features.

---

## Table of Contents

1. [ES6+ Modern Features](#es6-modern-features)
2. [Golf-Style Operators](#golf-style-operators)
3. [Alternative Syntax](#alternative-syntax)
4. [Quantum Computing (Experimental)](#quantum-computing-experimental)
5. [Generators and Iterators](#generators-and-iterators)
6. [Type Guards](#type-guards)

---

## ES6+ Modern Features

### Let/Const Bindings
```lpp
let x = 42;
let name = "LPP";
```

### Arrow Functions
```lpp
let double = x -> x * 2;
let add = (a, b) -> a + b;
let fat = x => x * 3;  // Fat arrow variant
```

### Rest Parameters
```lpp
fn sum(first: int, ...numbers) -> int {
    return first;
}
```

### Spread Operator
```lpp
let arr1 = [1, 2, 3];
let arr2 = [4, 5, 6];
let combined = [...arr1, ...arr2];
```

### Nullish Coalescing
```lpp
let value = nullable ?? defaultValue;
```

### Optional Chaining
```lpp
let name = obj?.person?.name;
```

### Destructuring
```lpp
// Arrays
let [a, b, c] = [1, 2, 3];

// Objects
let {x, y} = point;
```

### Template Literals
```lpp
let message = `Hello ${name}, you are ${age} years old`;
```

---

## Golf-Style Operators

Ultra-compact functional operators inspired by APL/J/K.

### Range Operator `~`
```lpp
0~10        // [0, 1, 2, 3, ..., 10]
0~10~2      // [0, 2, 4, 6, 8, 10] (step = 2)
10~0~-1     // [10, 9, 8, ..., 0] (descending)
```

### Map Operator `@`
```lpp
[1, 2, 3] @ (x => x * 2)     // [2, 4, 6]
[1, 2, 3] @ double           // Apply function
```

### Filter Operator `?`
```lpp
[1, 2, 3, 4, 5] ? (x => x > 2)    // [3, 4, 5]
numbers ? isEven                   // Filter with predicate
```

### Reduce Operator `\`
```lpp
[1, 2, 3, 4] \ ((acc, x) => acc + x)  // 10
numbers \ sum                          // Reduce with function
```

### Pipe Operator `|>`
```lpp
value |> double |> square |> format
// Equivalent to: format(square(double(value)))
```

### Paradigm Validation
Enable GOLFED paradigm for golf operator encouragement:
```lpp
#pragma paradigm GOLFED

let result = 0~100 ? (x => x % 2 == 0) @ (x => x * x) \ ((a, b) => a + b);
```

---

## Alternative Syntax

### Arrow-Left Return `<-`
```lpp
fn calculate(x: int) -> int {
    <- x * 2;  // Same as: return x * 2;
}
```

**Purpose:** More functional/mathematical style inspired by Haskell.

### Thin Arrow `->` vs Fat Arrow `=>`

**Thin Arrow `->` (Rust/TypeScript style):**
- Function return types
- Lambdas with explicit typing
```lpp
fn add(a: int, b: int) -> int { return a + b; }
let f: (int) -> int = x -> x * 2;
```

**Fat Arrow `=>` (JavaScript/Scala style):**
- Concise lambdas
- Type inference
```lpp
let doubled = nums @ (x => x * 2);
let filtered = nums ? (x => x > 0);
```

Both are **functionally identical** for lambdas - choose based on preference.

---

## Quantum Computing (Experimental)

**Status:** 🧪 Experimental  
**Enable:** `#pragma experimental quantum`

### Quantum Variables
Variables that exist in **superposition** (multiple states):

```lpp
#pragma experimental quantum

// Superposition of 6 states
quantum let dice = [1, 2, 3, 4, 5, 6];

// Operations apply to ALL states
quantum let doubled = dice.map(v => v * 2);

// Observe to collapse (random selection)
let result = doubled.observe();  // e.g., 8

// Subsequent reads return SAME value (consistency)
print(result);  // 8
print(result);  // 8 (stable!)
```

### Probability Weights
```lpp
// Weighted superposition
quantum let weighted: int = {
    1: 0.1,   // 10% chance
    2: 0.3,   // 30% chance
    3: 0.6    // 60% chance
};

let value = weighted.observe();
```

### Quantum Operations
```lpp
quantum let q = [1, 2, 3];

// Transform all states
let doubled = q.map(x => x * 2);      // [2, 4, 6] in superposition
let filtered = q.filter(x => x > 1); // [2, 3] in superposition

// Collapse and check
if (q.observe() > 1) {
    print("Greater than 1");
}
```

### Implementation
- Uses `std::discrete_distribution` for weighted random selection
- Probabilities automatically normalized (sum to 1.0)
- Collapsed values cached for consistency
- Thread-safe with `std::mt19937` RNG

**Use Cases:**
- Simulations with randomness
- Game mechanics (dice, loot drops)
- Monte Carlo algorithms
- Probabilistic modeling

---

## Generators and Iterators

### Yield Keyword
```lpp
fn* fibonacci(n: int) -> int {
    let a = 0;
    let b = 1;
    for (let i = 0; i < n; i++) {
        yield a;
        let temp = a;
        a = b;
        b = temp + b;
    }
}

// Use generator
for (let num in fibonacci(10)) {
    print(num);
}
```

### Generator Functions
- Marked with `fn*` syntax
- Use `yield` to produce values
- Transpile to C++20 coroutines (`co_yield`)
- Support iteration protocol

**Transpiled Output:**
```cpp
std::generator<int> fibonacci(int n) {
    int a = 0;
    int b = 1;
    for (int i = 0; i < n; i++) {
        co_yield a;
        int temp = a;
        a = b;
        b = temp + b;
    }
}
```

---

## Type Guards

### typeof Expression
```lpp
if (typeof x == "number") {
    print("x is a number");
}

let typeStr = typeof value;
```

**Transpiles to:** `typeid(x).name()` comparison

### instanceof Expression
```lpp
if (obj instanceof MyClass) {
    print("obj is an instance of MyClass");
}
```

**Transpiles to:** `dynamic_cast` check

### Guards in Switch Statements
```lpp
switch (value) {
    case 1 when value > 0:
        print("Positive 1");
        break;
    case x when typeof x == "string":
        print("String case");
        break;
    default:
        print("Other");
}
```

**when clause:** Additional condition for case matching

---

## Getters and Setters

### Property Accessors
```lpp
class Rectangle {
    let width: int;
    let height: int;
    
    get area() -> int {
        return this.width * this.height;
    }
    
    set area(value: int) {
        // Adjust dimensions to match area
        this.width = value / this.height;
    }
}

let rect = Rectangle{10, 5};
print(rect.area);      // Getter: 50
rect.area = 100;       // Setter
```

**Implementation:**
- `get` keyword marks getter functions
- `set` keyword marks setter functions
- Transpile to C++ methods with appropriate names

---

## Feature Comparison

| Feature | ES6 | L++ | Status |
|---------|-----|-----|--------|
| Arrow Functions | ✅ | ✅ | Full |
| Rest Parameters | ✅ | ✅ | Full |
| Spread Operator | ✅ | ✅ | Full |
| Destructuring | ✅ | ✅ | Full |
| Template Literals | ✅ | ✅ | Full |
| Nullish Coalescing | ✅ | ✅ | Full |
| Optional Chaining | ✅ | ✅ | Full |
| Generators | ✅ | ✅ | Full (C++20) |
| Type Guards | TypeScript | ✅ | Full |
| Quantum Variables | ❌ | ✅ | Experimental |
| Golf Operators | APL/K | ✅ | Full |
| Pipe Operator | F# | ✅ | Full |

---

## Paradigm Support

Enable paradigm-specific validations:

```lpp
// Encourage functional style
#pragma paradigm FUNCTIONAL

// Encourage golf-style operators
#pragma paradigm GOLFED

// Encourage explicit loops
#pragma paradigm IMPERATIVE

// Encourage OOP patterns
#pragma paradigm OOP

// Default: allow all styles
#pragma paradigm HYBRID
```

See [PARADIGMS.md](PARADIGMS.md) for full details.

---

## References

- [FULL_SPEC.md](FULL_SPEC.md) - Complete language specification
- [PARADIGMS.md](PARADIGMS.md) - Multi-paradigm programming guide
- [QUICKSTART.md](QUICKSTART.md) - Getting started guide
- [DESIGN_PATTERNS.md](DESIGN_PATTERNS.md) - Design patterns catalog

---

## Notes

**Stability Levels:**
- ✅ **Stable** - Production-ready
- 🧪 **Experimental** - Use with `#pragma experimental`
- 🚧 **Planned** - Not yet implemented

All stable features are fully tested and compiled to valid C++17/C++20 code.
