#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace lpp
{

    class Parser
    {
    public:
        explicit Parser(const std::vector<Token> &tokens);
        std::unique_ptr<Program> parse();

    private:
        std::vector<Token> tokens;
        size_t current = 0;

        Token peek() const;
        Token previous() const;
        Token advance();
        bool check(TokenType type) const;
        bool match(TokenType type);
        bool isAtEnd() const;
        Token consume(TokenType type, const std::string &message);

        void synchronize();
        void error(const std::string &message);

        // Parsing methods
        std::unique_ptr<Function> function();
        std::unique_ptr<ClassDecl> classDeclaration();
        std::unique_ptr<InterfaceDecl> interfaceDeclaration();
        std::unique_ptr<TypeDecl> typeDeclaration();

        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> varDeclaration();
        std::unique_ptr<Statement> ifStatement();
        std::unique_ptr<Statement> whileStatement();
        std::unique_ptr<Statement> returnStatement();
        std::unique_ptr<Statement> expressionStatement();
        std::vector<std::unique_ptr<Statement>> block();

        std::unique_ptr<Expression> expression();
        std::unique_ptr<Expression> nullishCoalescing();
        std::unique_ptr<Expression> logicalOr();
        std::unique_ptr<Expression> logicalAnd();
        std::unique_ptr<Expression> equality();
        std::unique_ptr<Expression> comparison();
        std::unique_ptr<Expression> term();
        std::unique_ptr<Expression> factor();
        std::unique_ptr<Expression> unary();
        std::unique_ptr<Expression> primary();
        std::unique_ptr<Expression> call();
    };

} // namespace lpp

#endif // PARSER_H
