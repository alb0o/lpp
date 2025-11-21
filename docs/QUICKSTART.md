# Quick Start Guide v0.8.17

## Prerequisites

- C++ compiler (g++ or clang++)
- CMake 3.15 or higher
- Git (optional)

## Building the Compiler

### On Windows (with Git Bash):
```bash
./build.bat
```

### On Linux/Mac:
```bash
chmod +x build.sh
./build.sh
```

### Manual Build:
```bash
mkdir build
cd build
cmake ..
cmake --build .
```

## Using the Compiler

### Compile and run a LPP program:
```bash
# Compile
./build/lppc examples/hello.lpp -o hello

# Run
./hello
```

### Generate C++ only (no compilation):
```bash
./build/lppc examples/hello.lpp -c
```

This will create `examples/hello.lpp.cpp` that you can inspect.

## Testing the Examples

### Hello World:
```bash
./build/lppc examples/hello.lpp -o hello
./hello
```

### Factorial:
```bash
./build/lppc examples/factorial.lpp -o factorial
./factorial
```

## Writing Your First Program

Create a file `test.lpp`:
```lpp
fn main() -> int {
    let x: int = 42;
    print(x);
    return 0;
}
```

Compile and run:
```bash
./build/lppc test.lpp -o test
./test
```

## Next Steps

- Read the full language specification: [FULL_SPEC.md](FULL_SPEC.md)
- Explore more examples in the `examples/` folder
- Contribute new features or report bugs
