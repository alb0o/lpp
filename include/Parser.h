#ifndef PARSER_H
#define PARSER_H

#include "Token.h"
#include "AST.h"
#include <vector>
#include <memory>

namespace lpp
{

    // FIX BUG #161: Parser lacks RAII guards for temporary allocations
    // TODO: Add scope guards for error recovery resources
    // - Use std::unique_ptr for temp AST nodes during parsing
    // - RAII guard for token stream state (restore on exception)
    // - ScopeGuard class for cleanup: auto guard = makeGuard([&]{ cleanup(); });
    // Example:
    //   struct ParserState {
    //     size_t tokenPos;
    //     ~ParserState() { /* restore state */ }
    //   };
    //   void parseExpr() {
    //     ParserState state(current);
    //     // On exception, state is auto-restored
    //   }
    class Parser
    {
    public:
        explicit Parser(const std::vector<Token> &tokens);
        explicit Parser(const std::vector<Token> &tokens, const std::string &sourceCode);
        std::unique_ptr<Program> parse();

        // Get accumulated errors
        const std::vector<std::string> &getErrors() const { return errors; }
        bool hasErrors() const { return !errors.empty(); }

    private:
        std::vector<Token> tokens;
        size_t current = 0;
        std::vector<std::string> errors;      // Accumulated parse errors
        bool panicMode = false;               // Error recovery state
        std::string sourceCode;               // Original source for error context
        std::vector<std::string> sourceLines; // Split by line for easy access

        // Helper to safely parse doubles with validation
        bool safeStod(const std::string &str, double &result)
        {
            if (str.empty())
                return false;

            // Check for multiple dots or invalid characters
            int dotCount = 0;
            int eCount = 0;
            for (size_t i = 0; i < str.length(); i++)
            {
                char c = str[i];
                if (c == '.')
                    dotCount++;
                else if (c == 'e' || c == 'E')
                    eCount++;
                else if (i == 0 && (c == '-' || c == '+'))
                    continue;
                else if (!std::isdigit(c))
                    return false;
            }

            if (dotCount > 1 || eCount > 1)
                return false;

            try
            {
                result = std::stod(str);
                // Check for infinity (MSVC compatible)
                if (result == std::numeric_limits<double>::infinity() ||
                    result == -std::numeric_limits<double>::infinity() ||
                    result != result) // NaN check
                {
                    return false;
                }
                return true;
            }
            catch (...)
            {
                return false;
            }
        }

        Token peek() const;
        Token peekNext() const;
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
        std::unique_ptr<ClassDecl> expandAutoPattern(std::unique_ptr<AutoPatternStmt> autoPattern);

        std::unique_ptr<Statement> statement();
        std::unique_ptr<Statement> varDeclaration();
        std::unique_ptr<Statement> quantumVarDeclaration();
        std::unique_ptr<Statement> ifStatement();
        std::unique_ptr<Statement> whileStatement();
        std::unique_ptr<Statement> forStatement();
        std::unique_ptr<Statement> doWhileStatement();
        std::unique_ptr<Statement> switchStatement();
        std::unique_ptr<Statement> tryCatchStatement();
        std::unique_ptr<Statement> enumDeclaration();
        std::unique_ptr<Statement> importStatement();
        std::unique_ptr<Statement> exportStatement();
        std::unique_ptr<Statement> returnStatement();
        std::unique_ptr<Statement> expressionStatement();
        std::vector<std::unique_ptr<Statement>> block(bool enableImplicitReturn = false);

        std::unique_ptr<Expression> expression();
        std::unique_ptr<Expression> nullishCoalescing();
        std::unique_ptr<Expression> logicalOr();
        std::unique_ptr<Expression> logicalAnd();
        std::unique_ptr<Expression> equality();
        std::unique_ptr<Expression> comparison();
        std::unique_ptr<Expression> symbolicOps();
        std::unique_ptr<Expression> term();
        std::unique_ptr<Expression> factor();
        std::unique_ptr<Expression> cast();
        std::unique_ptr<Expression> unary();
        std::unique_ptr<Expression> primary();
        std::unique_ptr<Expression> call();
    };

} // namespace lpp

#endif // PARSER_H
