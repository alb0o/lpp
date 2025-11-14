# LPP Language Support

Language support for LPP programming language in Visual Studio Code.

## Features

- **Syntax Highlighting**: Full syntax highlighting for LPP keywords, operators, types, and more
- **Error Detection**: Integrated problem matcher that displays compiler errors inline
- **Auto-completion**: Snippets for common LPP patterns (functions, loops, structs, etc.)
- **Bracket Matching**: Auto-closing and matching for brackets, parentheses, and quotes
- **Code Folding**: Support for folding code blocks and regions
- **Comment Support**: Line and block comment toggling

## Usage

1. Install the extension
2. Open any `.lpp` file
3. Syntax highlighting will be applied automatically
4. Use build tasks (Ctrl+Shift+B) to compile your LPP code

## Requirements

- LPP compiler (`lppc`) must be installed and in your PATH
- VS Code 1.80.0 or higher

## Extension Settings

This extension contributes the following settings:

* `lpp.compiler.path`: Path to the LPP compiler (default: `lppc`)

## Problem Matcher

The extension includes a problem matcher that parses LPP compiler output:

```
ERROR [DIV-BY-ZERO] Line 5 in function 'main': Division by zero detected
WARNING [DEAD-CODE] Line 10: Dead code detected
```

Errors and warnings will appear in the Problems panel and be underlined in the editor.

## Snippets

Available snippets:
- `fn` - Function definition
- `let` - Immutable variable binding
- `letm` - Mutable variable binding
- `if` / `ife` - If / If-else statements
- `while` / `for` - Loop constructs
- `struct` - Struct definition
- `main` - Main function
- `println` - Print statement

## Release Notes

### 0.1.0

Initial release:
- Syntax highlighting
- Problem matcher for compiler errors
- Code snippets
- Language configuration (brackets, comments, folding)

## Contributing

Report issues or contribute at: [GitHub repository URL]

## License

[Your License]
