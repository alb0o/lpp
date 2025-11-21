# Paradigm Enforcement in L++ v0.8.17

L++ is a **multiparadigm language** that supports imperative, functional, object-oriented, and hybrid programming styles. To prevent architectural chaos and maintain code consistency, **every file must explicitly declare its paradigm** using a pragma directive at the beginning.

## Why Paradigm Enforcement?

Multiparadigm languages offer flexibility, but without guidance, teams can end up with inconsistent codebases where each developer uses a different style. L++ solves this with **file-level paradigm declarations** that:

- ✅ Force **conscious architectural decisions**
- ✅ Make code **self-documenting** (you know the file's intent immediately)
- ✅ Enable **compile-time validation** of paradigm rules
- ✅ Allow **mixing paradigms** across files in the same project
- ✅ Provide a **smooth learning curve** (start with `hybrid`, graduate to stricter modes)

## Syntax

Every `.lpp` file must begin with:

```lpp
#pragma paradigm <mode>
```

Where `<mode>` is one of: `hybrid`, `functional`, `imperative`, or `oop`.

**Omitting the pragma results in a compile error:**
```
ERROR: Missing paradigm declaration. Add '#pragma paradigm <mode>' at the beginning of the file.
Valid modes: hybrid (recommended), functional, imperative, oop
```

---

## The Four Paradigms

### 1. `hybrid` - Everything Allowed (Recommended for Learning)

**Use when:** Prototyping, learning, or need full flexibility.

**Philosophy:** No restrictions. Use any feature combination.

**Allowed:**
- ✅ Mutable variables (`let mut x = 10`)
- ✅ Classes and OOP
- ✅ Golf operators (`@`, `?`, `\`)
- ✅ Iterate-while operators (`!!<`, `!!>`, `!! $`)
- ✅ Side effects and global state

**Example:**
```lpp
#pragma paradigm hybrid

class Counter {
    private mut count: int;
    
    fn increment() {
        this.count += 1;  // ✅ OK: mutation in hybrid
    }
}

fn main() {
    let data = [1,2,3] @ (x => x * 2);  // ✅ OK: golf operator
    let mut sum = 0;                     // ✅ OK: mutable variable
    for (let i in data) {
        sum += i;
    }
}
```

---

### 2. `functional` - Pure Functional Programming

**Use when:** Algorithms, mathematical computations, data transformations, maximum predictability.

**Philosophy:** Immutability, pure functions, no side effects. Inspired by Haskell/OCaml.

**Allowed:**
- ✅ Immutable variables (`let`, `const`)
- ✅ Pure functions (no side effects)
- ✅ Golf operators (`@`, `?`, `\`)
- ✅ Iterate-while operators
- ✅ Lambda expressions

**Forbidden:**
- ❌ Mutable variables (`let mut`)
- ❌ Variable reassignment
- ❌ Classes and OOP
- ❌ Global state mutation
- ❌ Side effects (I/O should be isolated)

**Example:**
```lpp
#pragma paradigm functional

fn factorial(n: int) -> int {
    return (n == 0) ? 1 : n * factorial(n - 1);  // ✅ OK: pure recursion
}

fn process_data(nums: [int]) -> [int] {
    return nums 
        ? (x => x > 0)    // ✅ OK: filter
        @ (x => x * x);   // ✅ OK: map
}

fn bad_example() {
    let mut x = 10;  // ❌ ERROR: Mutable variables not allowed in functional paradigm
    x = 20;          // ❌ ERROR: Variable reassignment not allowed
}

class Counter {}  // ❌ ERROR: Classes not allowed in functional paradigm
```

**Compiler Errors:**
```
ERROR: Mutable variables are not allowed in 'functional' paradigm. Use 'let' or 'const' instead.
ERROR: Variable reassignment is not allowed in 'functional' paradigm.
ERROR: Classes are not allowed in 'functional' paradigm. Use functions and data structures.
```

---

### 3. `imperative` - Performance-Oriented Control Flow

**Use when:** Performance-critical code, low-level operations, explicit control needed.

**Philosophy:** Manual control flow, explicit loops, minimal abstraction. Inspired by C/Go.

**Allowed:**
- ✅ Mutable variables
- ✅ Explicit loops (`for`, `while`)
- ✅ Explicit control flow
- ✅ Side effects
- ⚠️ Golf operators (allowed but **discouraged** with warnings)

**Forbidden:**
- ❌ Classes (use functions and structs)

**Example:**
```lpp
#pragma paradigm imperative

fn process_array(data: [mut int]) {
    let mut sum = 0;               // ✅ OK: mutable variable
    for (let mut i = 0; i < data.len(); i++) {
        data[i] *= 2;              // ✅ OK: mutation
        sum += data[i];
    }
    return sum;
}

fn with_golf_operators() {
    let result = [1,2,3] @ (x => x * 2);  
    // ⚠️ WARNING: Golf-style operator '@' is discouraged in imperative paradigm.
    //            Consider explicit loops.
}

class MyClass {}  // ❌ ERROR: Classes not allowed in imperative paradigm
```

**Compiler Warnings:**
```
WARNING: Golf-style operator '@' is discouraged in 'imperative' paradigm. Consider explicit loops.
         Imperative paradigm favors explicit control flow for clarity.
```

---

### 4. `oop` - Object-Oriented Programming

**Use when:** Domain modeling, enterprise architecture, encapsulation needed.

**Philosophy:** Classes, inheritance, encapsulation. Inspired by Java/C#.

**Allowed:**
- ✅ Classes and inheritance
- ✅ Encapsulation (private/public)
- ✅ Mutable state (inside objects)
- ✅ Golf operators (for data manipulation)
- ✅ Methods and constructors

**Example:**
```lpp
#pragma paradigm oop

class Shape {
    protected mut area: float;
    
    fn get_area() -> float {
        return this.area;
    }
}

class Circle extends Shape {
    private radius: float;
    
    constructor(r: float) {
        this.radius = r;
        this.area = 3.14159 * r * r;  // ✅ OK: mutation in constructor
    }
    
    fn scale(factor: float) {
        this.radius *= factor;         // ✅ OK: mutation in method
        this.area *= factor * factor;
    }
}

fn process_shapes(shapes: [Shape]) -> [float] {
    return shapes @ (s => s.get_area());  // ✅ OK: golf + OOP
}
```

---

## Paradigm Comparison Table

| Feature | `hybrid` | `functional` | `imperative` | `oop` |
|---------|----------|-------------|-------------|-------|
| Mutable variables (`mut`) | ✅ | ❌ | ✅ | ✅ |
| Variable reassignment | ✅ | ❌ | ✅ | ✅ |
| Classes | ✅ | ❌ | ❌ | ✅ |
| Golf operators (`@`,`?`,`\`) | ✅ | ✅ | ⚠️ warning | ✅ |
| Iterate-while (`!!<`,`!!>`) | ✅ | ✅ | ✅ | ✅ |
| Explicit loops | ✅ | ✅ | ✅ | ✅ |
| Side effects | ✅ | ❌ | ✅ | ✅ |
| Inheritance | ✅ | ❌ | ❌ | ✅ |

---

## Best Practices

### 1. **Start with `hybrid`, refine later**
   - Prototype with `hybrid` to experiment
   - Once code stabilizes, refactor into stricter paradigms
   - Use `functional` for pure business logic
   - Use `oop` for domain models
   - Use `imperative` for performance hot paths

### 2. **Organize files by paradigm**
   ```
   src/
     algorithms/       # functional paradigm
       sorting.lpp
       search.lpp
     models/          # oop paradigm
       User.lpp
       Order.lpp
     drivers/         # imperative paradigm
       file_io.lpp
       network.lpp
     utils/           # hybrid paradigm
       helpers.lpp
   ```

### 3. **When to use each paradigm:**

   **Functional** (`functional`):
   - Data transformations and pipelines
   - Algorithms and mathematical computations
   - Stateless services
   - Unit-testable pure logic

   **Imperative** (`imperative`):
   - Performance-critical loops
   - Low-level bit manipulation
   - Memory-sensitive operations
   - Systems programming

   **OOP** (`oop`):
   - Domain modeling (User, Product, Order)
   - Stateful entities
   - Polymorphic behavior
   - Enterprise design patterns

   **Hybrid** (`hybrid`):
   - Prototypes and experiments
   - Glue code between modules
   - Quick scripts
   - When multiple paradigms genuinely needed in one file

---

## Migration Guide

### From No Enforcement → Paradigm Enforcement

**Before (no pragma):**
```lpp
fn main() {
    let data = [1,2,3];
    // ...
}
```

**After (compilation error):**
```
ERROR: Missing paradigm declaration
```

**Fix (add pragma):**
```lpp
#pragma paradigm hybrid

fn main() {
    let data = [1,2,3];
    // ...
}
```

### Choosing the Right Paradigm

**Questions to ask:**
1. Does this file define classes? → `oop` or `hybrid`
2. Is this pure data transformation? → `functional`
3. Is performance critical with manual control? → `imperative`
4. Not sure / need flexibility? → `hybrid`

---

## Examples

### Example 1: Pure Functional Algorithm
```lpp
#pragma paradigm functional

fn fibonacci(n: int) -> [int] {
    return !!< 0, n 
        |> (seq => seq \ (a, b) => a + b, [0, 1]);
}

fn prime_sieve(limit: int) -> [int] {
    return (2 ~> limit) 
        ? (n => is_prime(n));
}

fn is_prime(n: int) -> bool {
    return (2 ~> sqrt(n)) @ (d => n % d != 0) \ (a, b) => a && b, true;
}
```

### Example 2: Object-Oriented Domain Model
```lpp
#pragma paradigm oop

class User {
    private mut points: int;
    private name: string;
    
    constructor(n: string) {
        this.name = n;
        this.points = 0;
    }
    
    fn add_points(p: int) {
        this.points += p;
    }
    
    fn get_level() -> string {
        return (this.points > 100) ? "Gold" : "Silver";
    }
}

class Admin extends User {
    private permissions: [string];
    
    fn grant_permission(perm: string) {
        this.permissions.push(perm);
    }
}
```

### Example 3: Performance-Critical Imperative Code
```lpp
#pragma paradigm imperative

fn matrix_multiply(a: [[float]], b: [[float]]) -> [[float]] {
    let n = a.len();
    let m = b[0].len();
    let mut result = create_matrix(n, m);
    
    for (let mut i = 0; i < n; i++) {
        for (let mut j = 0; j < m; j++) {
            let mut sum = 0.0;
            for (let mut k = 0; k < a[0].len(); k++) {
                sum += a[i][k] * b[k][j];
            }
            result[i][j] = sum;
        }
    }
    
    return result;
}
```

### Example 4: Hybrid Prototyping
```lpp
#pragma paradigm hybrid

class DataProcessor {
    private data: [int];
    
    fn filter_and_transform() {
        // Mix OOP + functional
        this.data = this.data 
            ? (x => x > 0)
            @ (x => x * x);
    }
}

fn quick_test() {
    let mut processor = DataProcessor([1, -2, 3, -4]);
    processor.filter_and_transform();
    
    // Imperative logging
    let mut i = 0;
    while (i < processor.data.len()) {
        print(processor.data[i]);
        i++;
    }
}
```

---

## Implementation Details

### Compiler Phases

1. **Lexer**: Tokenizes `#pragma paradigm <mode>` into `PRAGMA` token
2. **Parser**: Extracts paradigm mode, creates `Program` AST node with `ParadigmMode`
3. **Static Analyzer**: Validates code against paradigm rules during AST traversal
4. **Transpiler**: Unchanged (paradigm is analysis-only)

### Error Messages

**Missing Pragma:**
```
ERROR [line 1]: Missing paradigm declaration. Add '#pragma paradigm <mode>' at the beginning of the file.
Valid modes: hybrid (recommended), functional, imperative, oop
```

**Invalid Mode:**
```
ERROR [line 1]: Invalid paradigm mode 'funtional'. Expected: hybrid, functional, imperative, or oop
```

**Paradigm Violation:**
```
ERROR [line 15]: Mutable variables are not allowed in 'functional' paradigm. Use 'let' or 'const' instead.
NOTE: Functional paradigm enforces immutability for predictable behavior.
```

---

## FAQ

**Q: Can I have multiple paradigms in one file?**  
A: No. Each file has exactly one paradigm. This is by design to maintain consistency. If you need multiple styles, split into multiple files.

**Q: What if I absolutely need mutation in a functional file?**  
A: Either:
1. Switch to `hybrid` paradigm
2. Refactor to use immutable transformations
3. Isolate mutation in a separate `imperative` file and import the result

**Q: Why are golf operators discouraged in `imperative` but allowed?**  
A: They're allowed because they're powerful, but warnings remind you that explicit loops are more idiomatic in imperative style. It's guidance, not enforcement.

**Q: Can I configure paradigm rules in a config file?**  
A: Not currently. Pragma is file-level and explicit. This might be added in the future for team-level defaults.

**Q: Does this impact runtime performance?**  
A: No. Paradigm checking happens at compile-time only. The transpiled C++ is identical regardless of paradigm.

---

## Conclusion

Paradigm enforcement in L++ strikes a balance between **flexibility and discipline**. By requiring explicit paradigm declarations, L++ prevents architectural chaos while still allowing you to use the right tool for each job.

**Key Takeaways:**
- Every file must declare its paradigm with `#pragma paradigm <mode>`
- Four paradigms: `hybrid`, `functional`, `imperative`, `oop`
- Validation happens at compile-time with clear error messages
- Mix paradigms across files in the same project
- Start with `hybrid`, graduate to stricter modes as needed

For more information:
- [Golf Operators Documentation](ADVANCED_FEATURES.md#golf-operators)
- [Iterate-While Operators](ADVANCED_FEATURES.md#iterate-while)
- [Language Guide](../README.md)
