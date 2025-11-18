#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>

namespace lpp
{

    // FIX BUG #172: Lexer string buffer not reset on tokenization error
    // TODO: Use RAII for lexer state management
    // - StringBufferGuard: Clears buffer in destructor on exception
    // - Auto-reset position: LexerState saves/restores current position
    // - No memory leaks on partial tokenization
    // Example:
    //   struct LexerState {
    //     std::string& buffer;
    //     LexerState(std::string& b) : buffer(b) {}
    //     ~LexerState() { buffer.clear(); /* cleanup */ }
    //   };
    class Lexer
    {
    public:
        explicit Lexer(const std::string &source);
        std::vector<Token> tokenize();

    private:
        std::string source;
        size_t current = 0;
        int line = 1;
        int column = 1;

        char peek() const;
        char advance();
        bool isAtEnd() const;
        bool match(char expected);

        void skipWhitespace();
        void skipComment();
        void skipBlockComment();

        Token makeToken(TokenType type, const std::string &lexeme);
        Token number();
        Token string();
        Token identifier();
        Token pragma();

        TokenType identifierType(const std::string &text);
    };

} // namespace lpp

#endif // LEXER_H
