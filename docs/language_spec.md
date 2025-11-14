# LPP Language Specification v0.1

## Core del Linguaggio

### Tipi Base
- `int` - Numeri interi
- `float` - Numeri decimali
- `string` - Stringhe di testo
- `bool` - Booleani (true/false)
- `void` - Nessun valore

### Dichiarazione Variabili
```lpp
let x: int = 10;
let nome: string = "Hello";
let attivo: bool = true;
```

### Operatori

**Aritmetici:** `+`, `-`, `*`, `/`, `%`  
**Confronto:** `==`, `!=`, `<`, `>`, `<=`, `>=`  
**Logici:** `and`, `or`, `not`  
**Assegnamento:** `=`

### Strutture di Controllo

#### If-Else
```lpp
if (condizione) {
    // codice
} else {
    // codice
}
```

#### While
```lpp
while (condizione) {
    // codice
}
```

#### For
```lpp
for (let i: int = 0; i < 10; i = i + 1) {
    // codice
}
```

### Funzioni
```lpp
fn somma(a: int, b: int) -> int {
    return a + b;
}
```

### Punto di Ingresso
```lpp
fn main() -> int {
    // codice principale
    return 0;
}
```

### Commenti
```lpp
// commento su singola riga

/* commento
   multilinea */
```

### I/O Base
```lpp
print("Hello, World!");
let input: string = read();
```

## Sintassi EBNF (Minimale)

```ebnf
Program        = { Statement } ;
Statement      = Declaration | Assignment | IfStmt | WhileStmt | ForStmt | FunctionCall | Return ;
Declaration    = "let" Identifier ":" Type [ "=" Expression ] ";" ;
Assignment     = Identifier "=" Expression ";" ;
IfStmt         = "if" "(" Expression ")" Block [ "else" Block ] ;
WhileStmt      = "while" "(" Expression ")" Block ;
ForStmt        = "for" "(" Declaration Expression ";" Assignment ")" Block ;
Return         = "return" [ Expression ] ";" ;
Block          = "{" { Statement } "}" ;
Expression     = LogicalOr ;
LogicalOr      = LogicalAnd { "or" LogicalAnd } ;
LogicalAnd     = Equality { "and" Equality } ;
Equality       = Comparison { ( "==" | "!=" ) Comparison } ;
Comparison     = Term { ( "<" | ">" | "<=" | ">=" ) Term } ;
Term           = Factor { ( "+" | "-" ) Factor } ;
Factor         = Unary { ( "*" | "/" | "%" ) Unary } ;
Unary          = [ "not" | "-" ] Primary ;
Primary        = Number | String | Bool | Identifier | FunctionCall | "(" Expression ")" ;
FunctionCall   = Identifier "(" [ Expression { "," Expression } ] ")" ;
Function       = "fn" Identifier "(" [ Parameter { "," Parameter } ] ")" "->" Type Block ;
Parameter      = Identifier ":" Type ;
Type           = "int" | "float" | "string" | "bool" | "void" ;
Identifier     = Letter { Letter | Digit | "_" } ;
Number         = Digit { Digit } [ "." Digit { Digit } ] ;
String         = '"' { Character } '"' ;
Bool           = "true" | "false" ;
```

## Esempi

### Hello World
```lpp
fn main() -> int {
    print("Hello, World!");
    return 0;
}
```

### Calcolo Fattoriale
```lpp
fn factorial(n: int) -> int {
    if (n <= 1) {
        return 1;
    } else {
        return n * factorial(n - 1);
    }
}

fn main() -> int {
    let result: int = factorial(5);
    print(result);
    return 0;
}
```
