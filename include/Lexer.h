#ifndef LEXER_H
#define LEXER_H

#include "Token.h"
#include <vector>
#include <string>

namespace lpp
{

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

        Token makeToken(TokenType type, const std::string &lexeme);
        Token number();
        Token string();
        Token identifier();

        TokenType identifierType(const std::string &text);
    };

} // namespace lpp

#endif // LEXER_H
