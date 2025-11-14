# Golf-Style Symbolic Operators

LPP includes a set of symbolic operators inspired by APL/J/K for ultra-compact functional programming.

## Overview

Golf-style operators provide concise alternatives to verbose functional patterns:

| Operator | Name | LPP Syntax | Equivalent |
|----------|------|------------|------------|
| `~` | Range | `0~10` | `range(0, 10)` |
| `@` | Map | `arr @ fn` | `map(arr, fn)` |
| `?` | Filter | `arr ? pred` | `filter(arr, pred)` |
| `\` | Reduce | `arr \ fn` | `reduce(arr, fn)` |

## Range Operator `~`

Creates sequences of integers with optional step.

### Syntax

```lpp
start ~ end           // Step = 1
start ~ end ~ step    // Custom step
```

### Examples

```lpp
// Basic range
let r1 = 0~5;         // [0, 1, 2, 3, 4, 5]
let r2 = 1~10;        // [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

// With step
let evens = 0~10~2;   // [0, 2, 4, 6, 8, 10]
let odds = 1~10~2;    // [1, 3, 5, 7, 9]

// Descending
let down = 10~0~-1;   // [10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0]
```

### Implementation

Transpiles to IIFE generating `std::vector<int>`:

```cpp
([&]() { 
    std::vector<int> __range; 
    int __start = 0; 
    int __end = 5; 
    int __step = 1; 
    if (__step > 0) { 
        for (int i = __start; i <= __end; i += __step) 
            __range.push_back(i); 
    } else { 
        for (int i = __start; i >= __end; i += __step) 
            __range.push_back(i); 
    } 
    return __range; 
})()
```

## Map Operator `@`

Applies a function to each element of a collection.

### Syntax

```lpp
iterable @ function
```

### Examples

```lpp
let nums = [1, 2, 3, 4, 5];

// Double each element
let doubled = nums @ (x -> x * 2);
// [2, 4, 6, 8, 10]

// Square each element
let squared = nums @ (x -> x * x);
// [1, 4, 9, 16, 25]

// Chaining with ranges
let powers = 1~5 @ (n -> 2 ** n);
// [2, 4, 8, 16, 32]
```

### Implementation

Transpiles to IIFE with transform pattern:

```cpp
([&]() { 
    std::vector<decltype(fn(std::declval<T>()))> __result; 
    for (auto& __item : iterable) { 
        __result.push_back(fn(__item)); 
    } 
    return __result; 
})()
```

## Filter Operator `?`

Selects elements matching a predicate.

### Syntax

```lpp
iterable ? predicate
```

### Examples

```lpp
let nums = 1~20;

// Even numbers
let evens = nums ? (x -> x % 2 == 0);
// [2, 4, 6, 8, 10, 12, 14, 16, 18, 20]

// Greater than 10
let gt10 = nums ? (x -> x > 10);
// [11, 12, 13, 14, 15, 16, 17, 18, 19, 20]

// Divisible by 3
let div3 = nums ? (x -> x % 3 == 0);
// [3, 6, 9, 12, 15, 18]
```

### Implementation

Transpiles to IIFE with filter pattern:

```cpp
([&]() { 
    std::remove_reference_t<decltype(iterable)> __result; 
    for (auto& __item : iterable) { 
        if (predicate(__item)) { 
            __result.push_back(__item); 
        } 
    } 
    return __result; 
})()
```

**Note**: The `?` operator uses lookahead parsing to distinguish from ternary `?:`. 
If followed by a lambda (`|param|`), it's treated as filter; otherwise as ternary.

## Reduce Operator `\`

Folds a collection into a single value using an accumulator function.

### Syntax

```lpp
iterable \ function
```

### Examples

```lpp
let nums = 1~10;

// Sum
let sum = nums \ ((acc, x) -> acc + x);
// 55

// Product
let product = nums \ ((acc, x) -> acc * x);
// 3628800

// Maximum
let max = nums \ ((acc, x) -> acc > x ? acc : x);
// 10
```

### Implementation

Transpiles to IIFE with accumulate pattern:

```cpp
([&]() { 
    auto __acc = decltype(iterable)::value_type{}; 
    for (auto& __item : iterable) { 
        __acc = fn(__acc, __item); 
    } 
    return __acc; 
})()
```

## Composing Operators

Golf-style operators are designed to chain elegantly:

```lpp
// Sum of doubled numbers from 1 to 10
let result = (1~10 @ (x -> x * 2)) \ ((acc, x) -> acc + x);
// 110

// Sum of squares from 1 to 10
let result2 = (1~10 @ (x -> x * x)) \ ((acc, x) -> acc + x);
// 385

// Filter evens, square them, sum
let result3 = ((1~20 ? (x -> x % 2 == 0)) @ (x -> x * x)) \ ((acc, x) -> acc + x);
// 1540
```

## Performance Characteristics

All golf-style operators are **zero-cost abstractions**:

- Transpile to inline C++ code (IIFEs)
- No runtime function calls for the operator itself
- Lambda inlining by C++ compiler
- No heap allocations beyond vector creation

## Design Philosophy

The symbolic operators follow these principles:

1. **Semantic Clarity**: Each symbol has visual meaning
   - `~` suggests flow/range (waves)
   - `@` suggests "at each element"
   - `?` suggests conditional selection
   - `\` suggests folding/collapsing

2. **Composability**: Operators chain naturally left-to-right

3. **Consistency**: All operators return new collections (immutable style)

4. **Performance**: Zero-cost abstraction through inlining

5. **Familiarity**: Inspired by APL/J/K but using ASCII

## Comparison with Traditional Syntax

| Traditional | Golf-Style |
|-------------|------------|
| `for (int i = 0; i <= 5; i++) arr.push_back(i);` | `0~5` |
| `map(nums, double)` | `nums @ double` |
| `filter(nums, isEven)` | `nums ? isEven` |
| `reduce(nums, add, 0)` | `nums \ add` |

## Future Extensions

Planned additions:

- `>>` / `<<` - Function composition
- `??` - While loop shorthand (already implemented in lexer)
- `|>` - Pipeline operator (already implemented)

## See Also

- [examples/golf_syntax.lpp](../examples/golf_syntax.lpp) - Complete demonstration
- [AST.h](../include/AST.h) - AST node definitions
- [Parser.cpp](../src/Parser.cpp) - Parsing implementation
- [Transpiler.cpp](../src/Transpiler.cpp) - C++ code generation
