# LPP Language Specification v0.8.18
## Light Programming Language - Dual Syntax Edition

LPP is an innovative language with **dual syntax**:
- **Mode A**: JS-like (accessible, familiar)
- **Mode B**: Symbolic mathematics (elegant, compact)

Both modes compile to the same AST, zero overhead.

---

## 🎯 Design Philosophy

1. **JS-compatibility**: Everything that exists in JS must exist in LPP
2. **Mathematical symbolism**: Academic paper notation
3. **Category Theory**: Native functional composition
4. **Zero ceremonies**: Minimalist syntax where possible

---

## 📚 Table of Contents

1. [Base Types](#base-types)
2. [Variables](#variables)
3. [Functions](#functions)
4. [Control Flow](#control-flow)
5. [Symbolic Syntax](#symbolic-syntax)
6. [Advanced Features](#advanced-features)
7. [Operators](#operators)
8. [Objects and Classes](#objects-and-classes)
9. [Async/Await](#asyncawait)
10. [Modules](#modules)

---

## Base Types

```lpp
// Primitives
let n: int = 42
let f: float = 3.14
let s: string = "hello"
let b: bool = true
let nul: null = null
let undef: undefined = undefined

// Composites
let arr: int[] = [1, 2, 3]
let matrix: int[][] = [[1, 2], [3, 4]]
let tuple: [string, int] = ["age", 25]

// Objects
let obj: {name: string, age: int} = {name: "Alice", age: 30}

// Functions as type
let fn: (int, int) -> int = (a, b) -> a + b

// Any and Unknown
let any: any = "anything"
let unk: unknown = getValue()

// Union types
let val: string | int = 42

// Optional
let maybe: int? = null
```

---

## Variables

```lpp
// JS-like
let x = 10              // type inference
const PI = 3.14         // immutable
var legacy = "old"      // JS-like scope

// Destructuring
let [a, b, c] = [1, 2, 3]
let {name, age} = person
let [first, ...rest] = numbers

// Spread
let arr2 = [...arr1, 4, 5]
let obj2 = {...obj1, new: "value"}
```

---

## Functions

### Mode A: JS-like

```lpp
// Classic declaration
fn add(a: int, b: int) -> int {
    return a + b
}

// Arrow function
let double = (x: int) -> x * 2

// Arrow with body
let process = (data: string) -> {
    let result = transform(data)
    return result
}

// Default parameters
fn greet(name: string = "World") -> string {
    return "Hello, " + name
}

// Rest parameters
fn sum(...numbers: int[]) -> int {
    return numbers.reduce((a, b) -> a + b, 0)
}

// Generics
fn identity<T>(x: T) -> T {
    return x
}
```

### Mode B: Pure Mathematics

```lpp
// Mathematical definition (no keyword, no return)
double(x: int): int = x * 2

add(a: int, b: int): int = a + b

factorial(n: int): int = ?n<=1 -> 1 $ n * factorial(n-1)

// Minimal lambda (λ-calculus style)
x -> x * 2
(a, b) -> a + b

// With explicit types
(x: int): int -> x * 2
```

### Functional Composition (.)

```lpp
// Haskell-style composition
let pipeline = abs . sqrt . negate

// Equivalent to:
let pipeline = x -> abs(sqrt(negate(x)))

// Multiple levels
let transform = normalize . filter . map . parse

// Usage
let result = pipeline(data)
```

### Pipeline operator (|>)

```lpp
// F#/Elixir style
let result = data
    |> filter(x -> x > 0)
    |> map(x -> x * 2)
    |> reduce((a, b) -> a + b, 0)

// One-liner
let result = data |> process |> validate |> save
```

---

## Control Flow

### If-Else

#### Mode A: JS-like

```lpp
if (x > 10) {
    print("big")
} else if (x > 5) {
    print("medium")
} else {
    print("small")
}

// One-liner
if (x > 0) print("positive")
```

#### Mode B: Symbolic

```lpp
// Unary if (only then)
?x > 10 -> print("big")

// Binary if (then $ else)
?x > 10 -> "big" $ "small"

// Inline
let msg = ?x > 10 -> "big" $ "small"

// Nesting
?x > 10 -> "big" $ ?x > 5 -> "medium" $ "small"

// Piecewise (optional, pure ASCII)
x > 10 : "big" | "small"
```

### Loops

```lpp
// Classic for
for (let i = 0; i < 10; i++) {
    print(i)
}

// For-of (JS-like)
for (let item of array) {
    process(item)
}

// For-in (objects)
for (let key in object) {
    print(key, object[key])
}

// Range syntax
for (let i in 0..10) {
    print(i)
}

// Range with step
for (let i in 0..100..5) {  // 0, 5, 10, ..., 100
    print(i)
}

// Arrow-style for (compact)
for i in 0..10 -> print(i)

// While
while (condition) {
    process()
}

// Do-while
do {
    process()
} while (condition)
```

### Switch/Match

```lpp
// JS-like switch
switch (value) {
    case 1:
        print("one")
        break
    case 2:
        print("two")
        break
    default:
        print("other")
}

// Match (pattern matching)
match (value) {
    1 -> "one"
    2 -> "two"
    x if x > 10 -> "big"
    _ -> "other"
}

// Match with destructuring
match (tuple) {
    [0, _] -> "starts with zero"
    [_, 0] -> "ends with zero"
    [a, b] if a == b -> "equal"
    _ -> "different"
}
```

---

## Symbolic Syntax

### List Comprehension

```lpp
// Haskell-style
[x * 2 | x in 0..10]
[x * 2 | x in array, x > 5]
[x + y | x in 1..5, y in 1..5, x + y > 5]

// With multiple predicates
[x | x in numbers, x > 0, x % 2 == 0]

// Nested
[[x, y] | x in 1..3, y in 1..3]

// With Unicode (optional)
[x² | x ∈ 0..10, x > 3]
```

### Symbolic mathematical operators

```lpp
// Implication
cond → value        // ASCII: cond -> value

// Membership
x ∈ array           // ASCII: x in array

// Power
x² x³               // ASCII: x^2 x^3

// Root
√x ∛x               // ASCII: sqrt(x) cbrt(x)

// Infinity
∞                   // ASCII: Infinity

// Logical
∧ ∨ ¬               // ASCII: and or not
```

---

## Advanced Features

### Template Strings

```lpp
let name = "World"
let msg = `Hello, ${name}!`
let multiline = `
    Line 1
    Line 2
    ${expression}
`
```

### Advanced Destructuring

```lpp
// Nested
let {user: {name, age}} = data

// Default values
let {x = 0, y = 0} = point

// Rename
let {name: userName, age: userAge} = person

// Rest
let {first, second, ...others} = object
```

### Optional Chaining

```lpp
let value = obj?.property?.method?.()
let item = array?.[index]
```

### Nullish Coalescing

```lpp
let value = possiblyNull ?? defaultValue
```

### Closures and Scope

```lpp
fn makeCounter() -> () -> int {
    let count = 0
    return () -> {
        count++
        return count
    }
}

let counter = makeCounter()
print(counter())  // 1
print(counter())  // 2
```

### Higher-order functions

```lpp
// Map
let doubled = array.map(x -> x * 2)

// Filter
let evens = array.filter(x -> x % 2 == 0)

// Reduce
let sum = array.reduce((acc, x) -> acc + x, 0)

// Compose
let f = map(x -> x * 2) . filter(x -> x > 10) . reduce((a, b) -> a + b, 0)
```

---

## Operator Precedence and Notation System

### Philosophy

L++ adopts a **3-tier precedence system** that satisfies:
- **Beginners**: Standard mathematical precedence (multiplication before addition)
- **Scientists**: Custom operator precedence for DSLs
- **APL/Smalltalk fans**: Left-to-right evaluation without precedence

All within the same language, **without compilation flags**, using **lexical scoping**.

---

### Tier 0: Core Precedence (Fixed)

The default precedence table is **small and predictable**:

| Level | Operators | Associativity | Description |
|-------|-----------|---------------|-------------|
| 1 (highest) | `()` `[]` `.` | Left | Call, subscript, member access |
| 2 | `**` `^` `²` `³` | Right | Exponentiation |
| 3 | `∘` `.` (composition) | Right | Function composition |
| 4 | `*` `/` `%` `×` `÷` | Left | Multiplication, division |
| 5 | `+` `-` | Left | Addition, subtraction |
| 6 | `..` | Left | Range |
| 7 | `<` `>` `<=` `>=` `≤` `≥` | Left | Comparison |
| 8 | `==` `!=` `===` `!==` | Left | Equality |
| 9 | `∈` `in` | Left | Membership |
| 10 | `&&` `∧` `and` | Left | Logical AND |
| 11 | `||` `∨` `or` | Left | Logical OR |
| 12 | `?` `->` `$` | Right | Ternary conditional |
| 13 | `|>` | Left | Pipeline |
| 14 (lowest) | `=` `+=` `-=` etc. | Right | Assignment |

**Example:**
```lpp
// Standard mathematical precedence (default)
let x = 2 + 3 * 4       // = 14 (always)
let y = 2 ** 3 ** 2     // = 2^(3^2) = 512 (right-assoc)
let z = data |> filter |> map  // pipeline last
```

**Core operators (`+ - * / < > && ||`) cannot be redefined globally** - they are the foundation.

---

### Tier 1: Notation Blocks

**Notation blocks** allow you to change precedence rules **locally** for DSLs or alternative styles.

#### Mathematical Notation (default)

```lpp
notation math {
    // Standard precedence (default)
    let result = 2 + 3 * 4  // = 14
}
```

This is the **implicit default** - you don't need to declare it.

#### Linear Notation (Smalltalk/APL style)

```lpp
notation linear {
    // All operators: same precedence, left-associative
    let result = 2 + 3 * 4  // = (2 + 3) * 4 = 20
    let x = 5 - 2 + 1       // = (5 - 2) + 1 = 4
}
```

**Use case:** When you want predictable left-to-right evaluation without memorizing precedence tables.

#### Custom Notation (for DSLs)

```lpp
notation Algebra {
    infixl 7 ⊗, ⊙          // High precedence, left-assoc
    infixl 6 ⊕             // Lower precedence
    infix  5 ≡             // Non-associative
    
    let result = a ⊕ b ⊗ c  // = a ⊕ (b ⊗ c)
    let check = x ≡ y       // OK
    // let bad = x ≡ y ≡ z  // ERROR: non-associative
}
```

**Fixity declarations:**
- `infixl N op`: Left-associative, precedence level N
- `infixr N op`: Right-associative, precedence level N
- `infix N op`: Non-associative, precedence level N

Higher N = higher precedence (binds tighter).

---

### Tier 2: Inline Linear Expressions

For **single expressions** that need left-to-right evaluation:

```lpp
// Default: mathematical
let x = 2 + 3 * 4              // = 14

// Inline linear mode
let y = linear(2 + 3 * 4)      // = 20

// Useful in pipelines
let result = data
    |> filter(x -> x > 0)
    |> map(x -> linear(x + 1 * 2))  // (x + 1) * 2
```

**Semantics:** Inside `linear(...)`, all binary operators are treated as:
- Same precedence
- Left-associative

---

### Library Notation Export

Libraries can export **reusable notations**:

```lpp
// In algebra.lpp
export notation Algebra {
    infixl 7 ⊗
    infixl 6 ⊕
}

// In user code
import {notation Algebra} from "./algebra"

notation Algebra {
    let result = a ⊕ b ⊗ c  // Uses imported fixity
}
```

**Collision handling:**
```lpp
import {notation Math as MathA} from "./libA"
import {notation Math as MathB} from "./libB"

notation MathA {
    // Uses libA's definitions
}

notation MathB {
    // Uses libB's definitions
}
```

---

### Safety Rules

1. **Core operators are protected**
   ```lpp
   infixl 5 +  // ❌ ERROR: Cannot redefine core operator globally
   
   notation Custom {
       infixl 5 +  // ⚠️ WARNING: Redefining core operator locally
   }
   ```

2. **Notation blocks are lexically scoped**
   ```lpp
   let x = 2 + 3 * 4  // = 14
   
   notation linear {
       let y = 2 + 3 * 4  // = 20
   }
   
   let z = 2 + 3 * 4  // = 14 (back to default)
   ```

3. **Ambiguity warnings**
   ```lpp
   notation Custom {
       infixl 5 ⊕, ⊗  // Same precedence
       let x = a ⊕ b ⊗ c  // ⚠️ WARNING: Consider parentheses for clarity
   }
   ```

4. **Pretty-printer adds parentheses**
   - The formatter automatically adds `()` when using non-core operators
   - Helps readability without requiring precedence knowledge

---

### Examples

#### Mathematical DSL
```lpp
notation LinearAlgebra {
    infixl 7 ⊗        // Tensor product
    infixl 6 ⊕        // Direct sum
    infixr 5 ⊸        // Linear map
    
    let transform = (A ⊕ B) ⊸ (C ⊗ D)
}
```

#### Logic DSL
```lpp
notation Logic {
    infixr 3 →        // Implication
    infixl 4 ∧        // Conjunction
    infixl 3 ∨        // Disjunction
    
    let theorem = (p ∧ q) → (p ∨ r)
}
```

#### Mixed modes
```lpp
// Default: math
let a = 2 + 3 * 4  // = 14

// Block: linear
notation linear {
    let b = 2 + 3 * 4  // = 20
}

// Inline: linear
let c = linear(2 + 3 * 4)  // = 20

// Back to default
let d = 2 + 3 * 4  // = 14
```

---

## Operators

### Arithmetic
```lpp
+  -  *  /  %  **  // power
++  --             // increment/decrement
```

### Comparison
```lpp
==  !=  ===  !==   // strict/loose equality
<  >  <=  >=
```

### Logical
```lpp
and  or  not       // keywords
&&   ||  !         // symbols
∧    ∨   ¬         // Unicode (optional)
```

### Bitwise
```lpp
&  |  ^  ~  <<  >>  >>>
```

### Assignment
```lpp
=  +=  -=  *=  /=  %=  **=
&&=  ||=  ??=
```

### Spread/Rest
```lpp
...array
...object
```

---

## Objects and Classes

### Objects

```lpp
// Object literal
let person = {
    name: "Alice",
    age: 30,
    greet: () -> print("Hello!")
}

// Computed properties
let key = "dynamic"
let obj = {
    [key]: "value",
    ["key" + "2"]: "value2"
}

// Shorthand
let name = "Bob"
let obj = {name}  // {name: name}

// Methods
let obj = {
    method() {
        return "value"
    }
}
```

### Classes

```lpp
class Person {
    name: string
    age: int
    
    constructor(name: string, age: int) {
        this.name = name
        this.age = age
    }
    
    greet() -> string {
        return `Hello, I'm ${this.name}`
    }
    
    // Getter
    get info() -> string {
        return `${this.name} (${this.age})`
    }
    
    // Setter
    set age(value: int) {
        if (value >= 0) this.age = value
    }
    
    // Static
    static species = "Homo sapiens"
    
    static create(name: string) -> Person {
        return new Person(name, 0)
    }
}

// Inheritance
class Student extends Person {
    grade: int
    
    constructor(name: string, age: int, grade: int) {
        super(name, age)
        this.grade = grade
    }
    
    override greet() -> string {
        return super.greet() + `, grade ${this.grade}`
    }
}

// Abstract
abstract class Animal {
    abstract makeSound() -> string
}

// Interface
interface Drawable {
    draw() -> void
}

class Circle implements Drawable {
    draw() -> void {
        print("Drawing circle")
    }
}
```

---

## Async/Await

```lpp
// Promise
let promise: Promise<int> = fetchData()

// Async function
async fn getData() -> Promise<string> {
    let response = await fetch(url)
    let data = await response.json()
    return data
}

// Async arrow
let process = async () -> {
    return await compute()
}

// Error handling
try {
    let data = await getData()
} catch (error) {
    print(error)
} finally {
    cleanup()
}

// Promise combinators
Promise.all([p1, p2, p3])
Promise.race([p1, p2])
Promise.any([p1, p2])
```

---

## Modules

```lpp
// Export
export let value = 42
export fn helper() { }
export class MyClass { }

// Default export
export default fn main() { }

// Import
import {value, helper} from "./module"
import * as utils from "./utils"
import MyClass from "./class"

// Dynamic import
let module = await import("./dynamic")
```

---

## Complete Examples

### Example 1: Fibonacci (Dual)

```lpp
// Mode A: JS-like
fn fib(n: int) -> int {
    if (n <= 1) return n
    return fib(n - 1) + fib(n - 2)
}

// Mode B: Symbolic
fib(n: int): int = ?n <= 1 -> n $ fib(n-1) + fib(n-2)
```

### Example 2: Data Pipeline

```lpp
// Mode A
let result = numbers
    .filter(x -> x > 0)
    .map(x -> x * 2)
    .reduce((a, b) -> a + b, 0)

// Mode B
let result = numbers
    |> filter(x -> x > 0)
    |> map(x -> x * 2)
    |> reduce((a, b) -> a + b, 0)
```

### Example 3: Composition

```lpp
// Base functions
let double = x -> x * 2
let square = x -> x * x
let negate = x -> -x

// Composition
let transform = negate . square . double

// Usage
transform(5)  // -(10²) = -100
```

### Example 4: List Comprehension

```lpp
// Even numbers up to 20
let evens = [x | x in 0..20, x % 2 == 0]

// Cartesian product
let pairs = [[x, y] | x in 1..5, y in 1..5]

// With multiple filters
let primes = [x | x in 2..100, isPrime(x)]
```

---

## Complete EBNF

```ebnf
Program        = { Declaration } ;

Declaration    = FunctionDecl | ClassDecl | VarDecl | Import | Export ;

FunctionDecl   = "fn" Identifier [ TypeParams ] "(" [ Parameters ] ")" [ "->" Type ] Block
               | Identifier "(" [ Parameters ] ")" ":" Type "=" Expression ;

ClassDecl      = [ "abstract" ] "class" Identifier [ "extends" Type ] [ "implements" Types ] ClassBody ;

VarDecl        = ( "let" | "const" | "var" ) Pattern [ ":" Type ] [ "=" Expression ] ";" ;

Statement      = VarDecl | Assignment | IfStmt | MatchStmt | ForStmt | WhileStmt 
               | DoWhileStmt | TryCatch | Return | Break | Continue | ExprStmt ;

IfStmt         = "if" "(" Expression ")" Block [ "else" ( IfStmt | Block ) ]
               | "?" Expression "->" Expression [ "$" Expression ] ;

MatchStmt      = "match" "(" Expression ")" "{" { MatchArm } "}" ;

ForStmt        = "for" "(" ( VarDecl | Assignment ) ";" Expression ";" Expression ")" Block
               | "for" Pattern ( "of" | "in" ) Expression Block
               | "for" Pattern "in" Range [ "->" Expression ] ;

WhileStmt      = "while" "(" Expression ")" Block ;

Pipeline       = Expression { "|>" Expression } ;

Composition    = Expression { "." Expression } ;

ListComp       = "[" Expression "|" Pattern "in" Expression { "," Expression } "]" ;

Expression     = Pipeline | Composition | Ternary | Lambda | Primary ;

Lambda         = [ "(" Parameters ")" ] [ ":" Type ] "->" ( Expression | Block ) ;

Ternary        = "?" Expression "->" Expression "$" Expression ;

Primary        = Number | String | Bool | Identifier | Array | Object | Call 
               | "(" Expression ")" | ListComp ;

Range          = Expression ".." Expression [ ".." Expression ] ;
```

---

## Compilation Modes

```bash
# Default: accepts both syntaxes
lppc input.lpp -o output

# JS-like only (error on symbolic syntax)
lppc input.lpp --mode=js-only

# Symbolic only (error on JS syntax)
lppc input.lpp --mode=symbolic-only

# Convert from JS to symbolic
lppc input.lpp --convert=symbolic -o output.lpp

# Convert from symbolic to JS
lppc input.lpp --convert=js -o output.lpp
```

---

## Final Philosophy

LPP combines:
- ✅ **JavaScript familiarity** for rapid adoption
- ✅ **Mathematical elegance** for expressiveness
- ✅ **Category Theory** for powerful composition
- ✅ **Zero overhead** with transparent dual syntax
- ✅ **C++ transpilation** for native performance

**LPP = JavaScript + Haskell + Mathematical Notation + C++ Performance**
