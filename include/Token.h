#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <map>

namespace lpp
{

    enum class TokenType
    {
        // Literals
        NUMBER,
        STRING,
        TEMPLATE_STRING,
        IDENTIFIER,

        // Keywords
        LET,
        CONST,
        VAR,
        MUT,
        FN,
        TYPE,
        IF,
        ELSE,
        WHILE,
        FOR,
        DO,
        SWITCH,
        CASE,
        DEFAULT,
        BREAK,
        CONTINUE,
        RETURN,
        TRUE,
        FALSE,
        AND,
        OR,
        NOT,
        IN,
        OF,
        MATCH,
        CLASS,
        CONSTRUCTOR,
        EXTENDS,
        IMPLEMENTS,
        ABSTRACT,
        INTERFACE,
        STATIC,
        OVERRIDE,
        PUBLIC,
        PRIVATE,
        PROTECTED,
        THIS,
        SUPER,
        NEW,
        ASYNC,
        AWAIT,
        TRY,
        CATCH,
        FINALLY,
        THROW,
        IMPORT,
        EXPORT,
        FROM,
        AS,
        NULL_KW,
        UNDEFINED,

        // Types
        TYPE_INT,
        TYPE_FLOAT,
        TYPE_STRING,
        TYPE_BOOL,
        TYPE_VOID,
        TYPE_ANY,
        TYPE_UNKNOWN,

        // Operators
        PLUS,
        MINUS,
        STAR,
        SLASH,
        PERCENT,
        POWER, // **
        EQUAL,
        EQUAL_EQUAL,
        EQUAL_EQUAL_EQUAL, // ===
        BANG_EQUAL,
        BANG_EQUAL_EQUAL, // !==
        LESS,
        LESS_EQUAL,
        GREATER,
        GREATER_EQUAL,
        PLUS_PLUS,               // ++
        MINUS_MINUS,             // --
        PLUS_EQUAL,              // +=
        MINUS_EQUAL,             // -=
        STAR_EQUAL,              // *=
        SLASH_EQUAL,             // /=
        PERCENT_EQUAL,           // %=
        POWER_EQUAL,             // **=
        AMP_AMP,                 // &&
        PIPE_PIPE,               // ||
        AMP_AMP_EQUAL,           // &&=
        PIPE_PIPE_EQUAL,         // ||=
        QUESTION_QUESTION,       // ??
        QUESTION_QUESTION_EQUAL, // ??=

        // Bitwise
        AMP,                     // &
        PIPE,                    // |
        CARET,                   // ^
        TILDE,                   // ~
        BACKSLASH,               // \ (reduce/fold operator)
        LESS_LESS,               // <<
        GREATER_GREATER,         // >>
        GREATER_GREATER_GREATER, // >>>

        // Delimiters
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LBRACKET, // [
        RBRACKET, // ]
        SEMICOLON,
        COMMA,
        COLON,
        ARROW, // ->
        DOT,
        DOT_DOT,      // .. (range)
        DOT_DOT_DOT,  // ... (spread/rest)
        QUESTION,     // ? (ternary / optional chaining)
        QUESTION_DOT, // ?.
        DOLLAR,       // $ (else in ternary if)
        PIPE_GT,      // |> (pipeline)
        BACKTICK,     // ` (template strings)
        AT,           // @ (decorators)

        // Special
        END_OF_FILE,
        INVALID
    };

    struct Token
    {
        TokenType type;
        std::string lexeme;
        int line;
        int column;

        Token(TokenType t, const std::string &lex, int ln, int col)
            : type(t), lexeme(lex), line(ln), column(col) {}
    };

    // Helper for debugging
    std::string tokenTypeToString(TokenType type);

} // namespace lpp

#endif // TOKEN_H
