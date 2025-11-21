# LPP Language Specification v0.8.17
## Light Programming Language - Dual Syntax Edition

LPP è un linguaggio innovativo con **sintassi duale**:
- **Modalità A**: JS-like (accessibile, familiare)
- **Modalità B**: Matematica simbolica (elegante, compatta)

Entrambe le modalità compilano allo stesso AST, zero overhead.

---

## 🎯 Filosofia del Design

1. **JS-compatibility**: Tutto ciò che esiste in JS deve esistere in LPP
2. **Simbolismo matematico**: Notazione da paper accademici
3. **Category Theory**: Composizione funzionale nativa
4. **Zero cerimonie**: Sintassi minimalista dove possibile

---

## 📚 Indice

1. [Tipi Base](#tipi-base)
2. [Variabili](#variabili)
3. [Funzioni](#funzioni)
4. [Control Flow](#control-flow)
5. [Sintassi Simbolica](#sintassi-simbolica)
6. [Feature Avanzate](#feature-avanzate)
7. [Operatori](#operatori)
8. [Oggetti e Classi](#oggetti-e-classi)
9. [Async/Await](#asyncawait)
10. [Moduli](#moduli)

---

## Tipi Base

```lpp
// Primitivi
let n: int = 42
let f: float = 3.14
let s: string = "hello"
let b: bool = true
let nul: null = null
let undef: undefined = undefined

// Compositi
let arr: int[] = [1, 2, 3]
let matrix: int[][] = [[1, 2], [3, 4]]
let tuple: [string, int] = ["age", 25]

// Oggetti
let obj: {name: string, age: int} = {name: "Alice", age: 30}

// Funzioni come tipo
let fn: (int, int) -> int = (a, b) -> a + b

// Any e Unknown
let any: any = "anything"
let unk: unknown = getValue()

// Union types
let val: string | int = 42

// Optional
let maybe: int? = null
```

---

## Variabili

```lpp
// JS-like
let x = 10              // type inference
const PI = 3.14         // immutabile
var legacy = "old"      // scope JS-like

// Destructuring
let [a, b, c] = [1, 2, 3]
let {name, age} = person
let [first, ...rest] = numbers

// Spread
let arr2 = [...arr1, 4, 5]
let obj2 = {...obj1, new: "value"}
```

---

## Funzioni

### Modalità A: JS-like

```lpp
// Dichiarazione classica
fn add(a: int, b: int) -> int {
    return a + b
}

// Arrow function
let double = (x: int) -> x * 2

// Arrow con body
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

### Modalità B: Matematica Pura

```lpp
// Definizione matematica (no keyword, no return)
double(x: int): int = x * 2

add(a: int, b: int): int = a + b

factorial(n: int): int = ?n<=1 -> 1 $ n * factorial(n-1)

// Lambda minimale (stile λ-calculus)
x -> x * 2
(a, b) -> a + b

// Con tipi espliciti
(x: int): int -> x * 2
```

### Composizione funzionale (.)

```lpp
// Composizione Haskell-style
let pipeline = abs . sqrt . negate

// Equivale a:
let pipeline = x -> abs(sqrt(negate(x)))

// Più livelli
let transform = normalize . filter . map . parse

// Uso
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

#### Modalità A: JS-like

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

#### Modalità B: Simbolica

```lpp
// If unario (solo then)
?x > 10 -> print("big")

// If binario (then $ else)
?x > 10 -> "big" $ "small"

// Inline
let msg = ?x > 10 -> "big" $ "small"

// Nesting
?x > 10 -> "big" $ ?x > 5 -> "medium" $ "small"

// Piecewise (opzionale, ASCII puro)
x > 10 : "big" | "small"
```

### Loops

```lpp
// For classico
for (let i = 0; i < 10; i++) {
    print(i)
}

// For-of (JS-like)
for (let item of array) {
    process(item)
}

// For-in (oggetti)
for (let key in object) {
    print(key, object[key])
}

// Range syntax
for (let i in 0..10) {
    print(i)
}

// Range con step
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
// Switch JS-like
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

// Match con destructuring
match (tuple) {
    [0, _] -> "starts with zero"
    [_, 0] -> "ends with zero"
    [a, b] if a == b -> "equal"
    _ -> "different"
}
```

---

## Sintassi Simbolica

### List Comprehension

```lpp
// Haskell-style
[x * 2 | x in 0..10]
[x * 2 | x in array, x > 5]
[x + y | x in 1..5, y in 1..5, x + y > 5]

// Con predicati multipli
[x | x in numbers, x > 0, x % 2 == 0]

// Nested
[[x, y] | x in 1..3, y in 1..3]

// Con Unicode (opzionale)
[x² | x ∈ 0..10, x > 3]
```

### Operatori matematici simbolici

```lpp
// Implicazione
cond → value        // ASCII: cond -> value

// Appartenenza
x ∈ array           // ASCII: x in array

// Potenza
x² x³               // ASCII: x^2 x^3

// Radice
√x ∛x               // ASCII: sqrt(x) cbrt(x)

// Infinity
∞                   // ASCII: Infinity

// Logici
∧ ∨ ¬               // ASCII: and or not
```

---

## Feature Avanzate

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

### Destructuring avanzato

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

### Closures e Scope

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

## Operatori

### Aritmetici
```lpp
+  -  *  /  %  **  // potenza
++  --             // incremento/decremento
```

### Confronto
```lpp
==  !=  ===  !==   // uguaglianza stretta/loose
<  >  <=  >=
```

### Logici
```lpp
and  or  not       // keyword
&&   ||  !         // simboli
∧    ∨   ¬         // Unicode (opzionale)
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

## Oggetti e Classi

### Oggetti

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

### Classi

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

## Moduli

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

## Esempi Completi

### Esempio 1: Fibonacci (Duale)

```lpp
// Modalità A: JS-like
fn fib(n: int) -> int {
    if (n <= 1) return n
    return fib(n - 1) + fib(n - 2)
}

// Modalità B: Simbolica
fib(n: int): int = ?n <= 1 -> n $ fib(n-1) + fib(n-2)
```

### Esempio 2: Pipeline di Dati

```lpp
// Modalità A
let result = numbers
    .filter(x -> x > 0)
    .map(x -> x * 2)
    .reduce((a, b) -> a + b, 0)

// Modalità B
let result = numbers
    |> filter(x -> x > 0)
    |> map(x -> x * 2)
    |> reduce((a, b) -> a + b, 0)
```

### Esempio 3: Composizione

```lpp
// Funzioni base
let double = x -> x * 2
let square = x -> x * x
let negate = x -> -x

// Composizione
let transform = negate . square . double

// Uso
transform(5)  // -(10²) = -100
```

### Esempio 4: List Comprehension

```lpp
// Numeri pari fino a 20
let evens = [x | x in 0..20, x % 2 == 0]

// Prodotto cartesiano
let pairs = [[x, y] | x in 1..5, y in 1..5]

// Con filtri multipli
let primes = [x | x in 2..100, isPrime(x)]
```

---

## EBNF Completo

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

## Modalità di Compilazione

```bash
# Default: accetta entrambe le sintassi
lppc input.lpp -o output

# Solo JS-like (errore su sintassi simbolica)
lppc input.lpp --mode=js-only

# Solo simbolica (errore su sintassi JS)
lppc input.lpp --mode=symbolic-only

# Converti da JS a simbolica
lppc input.lpp --convert=symbolic -o output.lpp

# Converti da simbolica a JS
lppc input.lpp --convert=js -o output.lpp
```

---

## Filosofia Finale

LPP unisce:
- ✅ **Familiarità di JavaScript** per adozione rapida
- ✅ **Eleganza matematica** per espressività
- ✅ **Category Theory** per composizione potente
- ✅ **Zero overhead** con sintassi duale trasparente
- ✅ **Transpilazione C++** per performance native

**LPP = JavaScript + Haskell + Notazione Matematica + Performance C++**
