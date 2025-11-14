#include "Parser.h"
#include <iostream>
#include <stdexcept>

namespace lpp
{

    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    std::unique_ptr<Program> Parser::parse()
    {
        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<ClassDecl>> classes;
        std::vector<std::unique_ptr<InterfaceDecl>> interfaces;
        std::vector<std::unique_ptr<TypeDecl>> types;

        while (!isAtEnd())
        {
            if (check(TokenType::FN))
            {
                functions.push_back(function());
            }
            else if (check(TokenType::CLASS))
            {
                classes.push_back(classDeclaration());
            }
            else if (check(TokenType::INTERFACE))
            {
                interfaces.push_back(interfaceDeclaration());
            }
            else if (check(TokenType::TYPE))
            {
                types.push_back(typeDeclaration());
            }
            else
            {
                error("Expected function, class, interface, or type declaration");
                synchronize();
            }
        }

        return std::make_unique<Program>(std::move(functions), std::move(classes), std::move(interfaces), std::move(types));
    }

    Token Parser::peek() const
    {
        return tokens[current];
    }

    Token Parser::peekNext() const
    {
        if (current + 1 >= tokens.size())
            return tokens[tokens.size() - 1];
        return tokens[current + 1];
    }

    Token Parser::previous() const
    {
        return tokens[current - 1];
    }

    Token Parser::advance()
    {
        if (!isAtEnd())
            current++;
        return previous();
    }

    bool Parser::check(TokenType type) const
    {
        if (isAtEnd())
            return false;
        return peek().type == type;
    }

    bool Parser::match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }
        return false;
    }

    bool Parser::isAtEnd() const
    {
        return peek().type == TokenType::END_OF_FILE;
    }

    Token Parser::consume(TokenType type, const std::string &message)
    {
        if (check(type))
            return advance();
        error(message);
        throw std::runtime_error(message);
    }

    void Parser::synchronize()
    {
        advance();
        while (!isAtEnd())
        {
            if (previous().type == TokenType::SEMICOLON)
                return;
            if (peek().type == TokenType::FN)
                return;
            advance();
        }
    }

    void Parser::error(const std::string &message)
    {
        Token token = peek();
        std::cerr << "Parse error at line " << token.line << ", column " << token.column
                  << ": " << message << std::endl;
    }

    std::unique_ptr<Function> Parser::function()
    {
        consume(TokenType::FN, "Expected 'fn'");
        Token name = consume(TokenType::IDENTIFIER, "Expected function name");

        consume(TokenType::LPAREN, "Expected '(' after function name");

        // Parameters
        std::vector<std::pair<std::string, std::string>> parameters;
        bool hasRestParam = false;
        std::string restParamName = "";

        if (!check(TokenType::RPAREN))
        {
            do
            {
                // Check for rest parameter: ...args
                if (match(TokenType::DOT_DOT_DOT))
                {
                    Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name after '...'");
                    hasRestParam = true;
                    restParamName = paramName.lexeme;
                    break; // Rest param must be last
                }

                Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                consume(TokenType::COLON, "Expected ':' after parameter name");
                Token paramType = advance();
                parameters.push_back({paramName.lexeme, paramType.lexeme});
            } while (match(TokenType::COMMA));
        }

        consume(TokenType::RPAREN, "Expected ')' after parameters");
        consume(TokenType::ARROW, "Expected '->' after parameters");

        Token returnType = advance();

        auto body = block();

        return std::make_unique<Function>(name.lexeme, std::move(parameters),
                                          returnType.lexeme, std::move(body),
                                          hasRestParam, restParamName);
    }

    std::unique_ptr<Statement> Parser::statement()
    {
        if (match(TokenType::LET))
            return varDeclaration();
        if (match(TokenType::IF))
            return ifStatement();
        if (match(TokenType::WHILE))
            return whileStatement();
        if (match(TokenType::RETURN))
            return returnStatement();

        // Check for assignment vs expression statement
        if (check(TokenType::IDENTIFIER))
        {
            size_t saved = current;
            advance();
            if (match(TokenType::EQUAL))
            {
                // Assignment
                Token name = tokens[saved];
                auto value = expression();
                consume(TokenType::SEMICOLON, "Expected ';' after assignment");
                return std::make_unique<Assignment>(name.lexeme, std::move(value));
            }
            else
            {
                // Reset and parse as expression
                current = saved;
            }
        }

        return expressionStatement();
    }

    std::unique_ptr<Statement> Parser::varDeclaration()
    {
        // Check for 'mut' keyword
        bool isMutable = false;
        if (match(TokenType::MUT))
        {
            isMutable = true;
        }

        Token name = consume(TokenType::IDENTIFIER, "Expected variable name");

        std::string typeName = "auto"; // Default to auto for type inference
        if (isMutable)
        {
            typeName = "mut " + typeName; // Mark as mutable for borrow checker
        }

        if (match(TokenType::COLON))
        {
            Token type = advance();
            if (isMutable && typeName.find("mut") != std::string::npos)
            {
                typeName = "mut " + type.lexeme;
            }
            else
            {
                typeName = type.lexeme;
            }
        }

        std::unique_ptr<Expression> initializer = nullptr;
        if (match(TokenType::EQUAL))
        {
            initializer = expression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");
        return std::make_unique<VarDecl>(name.lexeme, typeName, std::move(initializer));
    }

    std::unique_ptr<Statement> Parser::ifStatement()
    {
        consume(TokenType::LPAREN, "Expected '(' after 'if'");
        auto condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after condition");

        auto thenBranch = block();
        std::vector<std::unique_ptr<Statement>> elseBranch;

        if (match(TokenType::ELSE))
        {
            elseBranch = block();
        }

        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
    }

    std::unique_ptr<Statement> Parser::whileStatement()
    {
        consume(TokenType::LPAREN, "Expected '(' after 'while'");
        auto condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after condition");

        auto body = block();

        return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
    }

    std::unique_ptr<Statement> Parser::returnStatement()
    {
        std::unique_ptr<Expression> value = nullptr;

        if (!check(TokenType::SEMICOLON))
        {
            value = expression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after return statement");
        return std::make_unique<ReturnStmt>(std::move(value));
    }

    std::unique_ptr<Statement> Parser::expressionStatement()
    {
        auto expr = expression();
        consume(TokenType::SEMICOLON, "Expected ';' after expression");
        return std::make_unique<ExprStmt>(std::move(expr));
    }

    std::vector<std::unique_ptr<Statement>> Parser::block()
    {
        consume(TokenType::LBRACE, "Expected '{'");

        std::vector<std::unique_ptr<Statement>> statements;
        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            statements.push_back(statement());
        }

        consume(TokenType::RBRACE, "Expected '}'");
        return statements;
    }

    std::unique_ptr<Expression> Parser::expression()
    {
        // Check for ternary if: ?cond -> a $ b
        if (match(TokenType::QUESTION))
        {
            auto condition = nullishCoalescing();
            consume(TokenType::ARROW, "Expected '->' after condition in ternary if");
            auto thenExpr = nullishCoalescing();

            if (match(TokenType::DOLLAR))
            {
                auto elseExpr = expression();
                return std::make_unique<TernaryIfExpr>(std::move(condition), std::move(thenExpr), std::move(elseExpr));
            }
            else
            {
                // If unario: ?cond -> expr (senza else)
                // Wrap in statement per ora, o genera ternary con nullptr
                return std::make_unique<TernaryIfExpr>(std::move(condition), std::move(thenExpr), nullptr);
            }
        }

        // Check for lambda: x -> expr or (params) -> expr
        size_t saved = current;
        if (check(TokenType::IDENTIFIER))
        {
            advance(); // consume identifier
            if (match(TokenType::ARROW))
            {
                // Single param lambda
                std::string paramName = tokens[saved].lexeme;
                std::vector<std::pair<std::string, std::string>> params = {{paramName, ""}};
                auto body = expression();
                return std::make_unique<LambdaExpr>(std::move(params), std::move(body));
            }
            else
            {
                current = saved; // reset
            }
        }
        else if (match(TokenType::LPAREN))
        {
            // Could be lambda or grouped expression
            // Try to parse as lambda params
            std::vector<std::pair<std::string, std::string>> params;
            bool isLambda = false;
            bool hasRestParam = false;
            std::string restParamName = "";

            if (!check(TokenType::RPAREN))
            {
                size_t lambdaStart = current;
                // Try parsing as param list
                do
                {
                    // Check for rest parameter: ...args
                    if (match(TokenType::DOT_DOT_DOT))
                    {
                        if (check(TokenType::IDENTIFIER))
                        {
                            Token paramName = advance();
                            hasRestParam = true;
                            restParamName = paramName.lexeme;
                            break; // Rest param must be last
                        }
                    }

                    if (check(TokenType::IDENTIFIER))
                    {
                        Token paramName = advance();
                        std::string type = "";
                        if (match(TokenType::COLON))
                        {
                            Token typeToken = advance();
                            type = typeToken.lexeme;
                        }
                        params.push_back({paramName.lexeme, type});
                    }
                    else
                    {
                        break;
                    }
                } while (match(TokenType::COMMA));

                if (match(TokenType::RPAREN) && match(TokenType::ARROW))
                {
                    // It's a lambda!
                    isLambda = true;
                    auto body = expression();
                    return std::make_unique<LambdaExpr>(std::move(params), std::move(body), "", hasRestParam, restParamName);
                }
                else
                {
                    // Not a lambda, reset and parse as grouped expr
                    current = saved;
                }
            }
        }

        // Pipeline: expr |> fn |> fn
        auto expr = nullishCoalescing();

        if (match(TokenType::PIPE_GT))
        {
            std::vector<std::unique_ptr<Expression>> stages;
            do
            {
                stages.push_back(nullishCoalescing());
            } while (match(TokenType::PIPE_GT));

            return std::make_unique<PipelineExpr>(std::move(expr), std::move(stages));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::nullishCoalescing()
    {
        auto expr = logicalOr();

        while (match(TokenType::QUESTION_QUESTION))
        {
            auto right = logicalOr();
            // a ?? b => (a != null ? a : b) - but we don't have null yet, use simple OR for now
            // Generate: a ? a : b (but this re-evaluates a)
            // Better: use BinaryExpr with "??" and handle in transpiler
            expr = std::make_unique<BinaryExpr>(std::move(expr), "??", std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::logicalOr()
    {
        auto expr = logicalAnd();

        while (match(TokenType::OR))
        {
            std::string op = previous().lexeme;
            auto right = logicalAnd();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::logicalAnd()
    {
        auto expr = equality();

        while (match(TokenType::AND))
        {
            std::string op = previous().lexeme;
            auto right = equality();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::equality()
    {
        auto expr = comparison();

        while (match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL))
        {
            std::string op = previous().lexeme;
            auto right = comparison();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::comparison()
    {
        auto expr = symbolicOps();

        while (match(TokenType::LESS) || match(TokenType::LESS_EQUAL) ||
               match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL))
        {
            std::string op = previous().lexeme;
            auto right = symbolicOps();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::symbolicOps()
    {
        auto expr = term();

        // Map operator: arr @ fn
        if (match(TokenType::AT))
        {
            auto fn = term();
            return std::make_unique<MapExpr>(std::move(expr), std::move(fn));
        }

        // Filter operator: arr ? |x| condition (but not ternary ?)
        // We need to check if next is a lambda to distinguish from ternary
        if (check(TokenType::QUESTION) && peekNext().type == TokenType::PIPE)
        {
            advance(); // consume ?
            auto predicate = term();
            return std::make_unique<FilterExpr>(std::move(expr), std::move(predicate));
        }

        // Reduce operator: arr \ |acc, x| expr
        if (match(TokenType::BACKSLASH))
        {
            auto fn = term();
            return std::make_unique<ReduceExpr>(std::move(expr), std::move(fn));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::term()
    {
        auto expr = factor();

        // Check for range operator: start..end or start..end..step or start~end or start~end~step
        if (match(TokenType::DOT_DOT) || match(TokenType::TILDE))
        {
            auto end = factor();
            std::unique_ptr<Expression> step = nullptr;

            if (match(TokenType::DOT_DOT) || match(TokenType::TILDE))
            {
                step = factor();
            }

            return std::make_unique<RangeExpr>(std::move(expr), std::move(end), std::move(step));
        }

        while (match(TokenType::PLUS) || match(TokenType::MINUS))
        {
            std::string op = previous().lexeme;
            auto right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::factor()
    {
        auto expr = unary();

        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT))
        {
            std::string op = previous().lexeme;
            auto right = unary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::unary()
    {
        if (match(TokenType::NOT) || match(TokenType::MINUS))
        {
            std::string op = previous().lexeme;
            auto operand = unary();
            return std::make_unique<UnaryExpr>(op, std::move(operand));
        }

        // Function composition: f . g . h
        auto expr = call();

        // Composition check: identifier . identifier (without call)
        // Only compose if we have identifier followed by DOT and another identifier
        if (dynamic_cast<IdentifierExpr *>(expr.get()) && check(TokenType::DOT))
        {
            size_t savedPos = current;
            advance(); // consume DOT

            if (check(TokenType::IDENTIFIER))
            {
                // Could be composition: f . g
                // Create composition
                std::vector<std::unique_ptr<Expression>> functions;
                functions.push_back(std::move(expr));

                current = savedPos; // reset to before DOT
                do
                {
                    match(TokenType::DOT); // consume DOT
                    auto nextFunc = call();
                    functions.push_back(std::move(nextFunc));
                } while (dynamic_cast<IdentifierExpr *>(functions.back().get()) && check(TokenType::DOT));

                if (functions.size() > 1)
                {
                    return std::make_unique<CompositionExpr>(std::move(functions));
                }
                else
                {
                    // Just one function, return it
                    return std::move(functions[0]);
                }
            }
            else
            {
                current = savedPos; // reset
            }
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::call()
    {
        auto expr = primary();

        // Handle member access (. and ?.) and function calls
        while (true)
        {
            if (match(TokenType::DOT))
            {
                // obj.prop or obj.method()
                Token propName = consume(TokenType::IDENTIFIER, "Expected property name after '.'");
                auto propExpr = std::make_unique<IdentifierExpr>(propName.lexeme);
                expr = std::make_unique<IndexExpr>(std::move(expr), std::move(propExpr), true, false);
            }
            else if (match(TokenType::QUESTION_DOT))
            {
                // obj?.prop (optional chaining)
                Token propName = consume(TokenType::IDENTIFIER, "Expected property name after '?.'");
                auto propExpr = std::make_unique<IdentifierExpr>(propName.lexeme);
                expr = std::make_unique<IndexExpr>(std::move(expr), std::move(propExpr), true, true);
            }
            else if (match(TokenType::LBRACKET))
            {
                // arr[index]
                auto indexExpr = expression();
                consume(TokenType::RBRACKET, "Expected ']' after index");
                expr = std::make_unique<IndexExpr>(std::move(expr), std::move(indexExpr), false, false);
            }
            else if (check(TokenType::LPAREN) && dynamic_cast<IdentifierExpr *>(expr.get()))
            {
                // Function call
                std::string functionName = dynamic_cast<IdentifierExpr *>(expr.get())->name;
                advance(); // consume '('

                std::vector<std::unique_ptr<Expression>> arguments;
                if (!check(TokenType::RPAREN))
                {
                    do
                    {
                        arguments.push_back(expression());
                    } while (match(TokenType::COMMA));
                }

                consume(TokenType::RPAREN, "Expected ')' after arguments");
                expr = std::make_unique<CallExpr>(functionName, std::move(arguments));
            }
            else
            {
                break;
            }
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::primary()
    {
        if (match(TokenType::NUMBER))
        {
            return std::make_unique<NumberExpr>(std::stod(previous().lexeme));
        }

        if (match(TokenType::STRING))
        {
            return std::make_unique<StringExpr>(previous().lexeme);
        }

        // Template literal: `text ${expr} more`
        if (match(TokenType::BACKTICK))
        {
            std::vector<std::string> strings;
            std::vector<std::unique_ptr<Expression>> interpolations;
            std::string currentString;

            // Parse template content manually from source
            while (!check(TokenType::BACKTICK) && !isAtEnd())
            {
                char c = peek().lexeme[0];

                if (c == '$' && current + 1 < tokens.size() && tokens[current + 1].type == TokenType::LBRACE)
                {
                    // Found ${
                    strings.push_back(currentString);
                    currentString.clear();

                    advance(); // skip $
                    advance(); // skip {

                    // Parse expression inside ${}
                    auto expr = expression();
                    interpolations.push_back(std::move(expr));

                    consume(TokenType::RBRACE, "Expected '}' after interpolation");
                }
                else
                {
                    // Regular character - accumulate in currentString
                    // For now, use identifier/string tokens as placeholders
                    if (check(TokenType::IDENTIFIER) || check(TokenType::STRING) || check(TokenType::NUMBER))
                    {
                        currentString += peek().lexeme;
                        advance();
                    }
                    else
                    {
                        break;
                    }
                }
            }

            strings.push_back(currentString); // Final string part
            consume(TokenType::BACKTICK, "Expected '`' after template literal");

            return std::make_unique<TemplateLiteralExpr>(std::move(strings), std::move(interpolations));
        }

        if (match(TokenType::TRUE))
        {
            return std::make_unique<BoolExpr>(true);
        }

        if (match(TokenType::FALSE))
        {
            return std::make_unique<BoolExpr>(false);
        }

        if (match(TokenType::IDENTIFIER))
        {
            return std::make_unique<IdentifierExpr>(previous().lexeme);
        }

        // Array literal or list comprehension: [...]
        if (match(TokenType::LBRACKET))
        {
            // Empty array
            if (check(TokenType::RBRACKET))
            {
                advance();
                return std::make_unique<ArrayExpr>(std::vector<std::unique_ptr<Expression>>());
            }

            // Check for spread: [...arr]
            if (check(TokenType::DOT_DOT_DOT))
            {
                // Array with spread elements
                std::vector<std::unique_ptr<Expression>> elements;

                do
                {
                    if (match(TokenType::DOT_DOT_DOT))
                    {
                        auto spreadExpr = expression();
                        elements.push_back(std::make_unique<SpreadExpr>(std::move(spreadExpr)));
                    }
                    else
                    {
                        elements.push_back(expression());
                    }
                } while (match(TokenType::COMMA) && !check(TokenType::RBRACKET));

                consume(TokenType::RBRACKET, "Expected ']' after array elements");
                return std::make_unique<ArrayExpr>(std::move(elements));
            }

            // Check if it's a list comprehension: [expr | var in range, ...]
            size_t saved = current;
            auto firstExpr = expression();

            if (match(TokenType::PIPE))
            {
                // List comprehension!
                Token varName = consume(TokenType::IDENTIFIER, "Expected variable name in list comprehension");
                consume(TokenType::IN, "Expected 'in' in list comprehension");
                auto range = expression();

                std::vector<std::unique_ptr<Expression>> predicates;
                while (match(TokenType::COMMA))
                {
                    predicates.push_back(expression());
                }

                consume(TokenType::RBRACKET, "Expected ']' after list comprehension");
                return std::make_unique<ListComprehension>(std::move(firstExpr), varName.lexeme, std::move(range), std::move(predicates));
            }
            else
            {
                // Regular array literal - can contain spread elements
                std::vector<std::unique_ptr<Expression>> elements;
                elements.push_back(std::move(firstExpr));

                while (match(TokenType::COMMA))
                {
                    if (check(TokenType::RBRACKET))
                        break; // trailing comma

                    if (match(TokenType::DOT_DOT_DOT))
                    {
                        auto spreadExpr = expression();
                        elements.push_back(std::make_unique<SpreadExpr>(std::move(spreadExpr)));
                    }
                    else
                    {
                        elements.push_back(expression());
                    }
                }

                consume(TokenType::RBRACKET, "Expected ']' after array elements");
                return std::make_unique<ArrayExpr>(std::move(elements));
            }
        }

        // Match expression: match expr { case pattern -> result }
        if (match(TokenType::MATCH))
        {
            auto expr = expression();
            consume(TokenType::LBRACE, "Expected '{' after match expression");

            std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> cases;

            while (!check(TokenType::RBRACE) && !isAtEnd())
            {
                consume(TokenType::CASE, "Expected 'case' in match expression");
                auto pattern = expression();
                consume(TokenType::ARROW, "Expected '->' after pattern");
                auto result = expression();
                cases.push_back({std::move(pattern), std::move(result)});

                // Optional semicolon or comma
                match(TokenType::SEMICOLON);
                match(TokenType::COMMA);
            }

            consume(TokenType::RBRACE, "Expected '}' after match cases");
            return std::make_unique<MatchExpr>(std::move(expr), std::move(cases));
        }

        // Object literal: {name: value, ...} or {name, age} (shorthand)
        if (match(TokenType::LBRACE))
        {
            std::vector<std::pair<std::string, std::unique_ptr<Expression>>> properties;

            while (!check(TokenType::RBRACE) && !isAtEnd())
            {
                if (check(TokenType::RBRACE))
                    break; // trailing comma

                Token key = consume(TokenType::IDENTIFIER, "Expected property name");

                // Check for shorthand: {name} => {name: name}
                if (match(TokenType::COMMA) || check(TokenType::RBRACE))
                {
                    // Shorthand property
                    properties.push_back({key.lexeme, std::make_unique<IdentifierExpr>(key.lexeme)});
                }
                else
                {
                    // Full property: key: value
                    consume(TokenType::COLON, "Expected ':' after property name");
                    auto value = expression();
                    properties.push_back({key.lexeme, std::move(value)});

                    if (!check(TokenType::RBRACE))
                    {
                        consume(TokenType::COMMA, "Expected ',' or '}' after property");
                    }
                }
            }

            consume(TokenType::RBRACE, "Expected '}' after object properties");
            return std::make_unique<ObjectExpr>(std::move(properties));
        }

        if (match(TokenType::LPAREN))
        {
            auto expr = expression();
            consume(TokenType::RPAREN, "Expected ')' after expression");
            return expr;
        }

        error("Expected expression");
        throw std::runtime_error("Expected expression");
    }

    std::unique_ptr<ClassDecl> Parser::classDeclaration()
    {
        consume(TokenType::CLASS, "Expected 'class'");
        Token name = consume(TokenType::IDENTIFIER, "Expected class name");

        std::string baseClass;
        if (match(TokenType::EXTENDS))
        {
            Token base = consume(TokenType::IDENTIFIER, "Expected base class name");
            baseClass = base.lexeme;
        }

        consume(TokenType::LBRACE, "Expected '{' after class name");

        std::vector<std::pair<std::string, std::string>> properties;
        std::vector<std::unique_ptr<Function>> methods;
        std::unique_ptr<Function> constructor = nullptr;

        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            // Constructor
            if (match(TokenType::CONSTRUCTOR))
            {
                consume(TokenType::LPAREN, "Expected '(' after 'constructor'");
                std::vector<std::pair<std::string, std::string>> params;

                if (!check(TokenType::RPAREN))
                {
                    do
                    {
                        Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                        consume(TokenType::COLON, "Expected ':' after parameter name");
                        Token paramType = advance();
                        params.push_back({paramName.lexeme, paramType.lexeme});
                    } while (match(TokenType::COMMA));
                }

                consume(TokenType::RPAREN, "Expected ')' after parameters");
                auto body = block();
                constructor = std::make_unique<Function>(name.lexeme, std::move(params), "void", std::move(body));
            }
            // Method
            else if (check(TokenType::FN))
            {
                methods.push_back(function());
            }
            // Property
            else if (check(TokenType::IDENTIFIER))
            {
                Token propName = advance();
                consume(TokenType::COLON, "Expected ':' after property name");
                Token propType = advance();
                consume(TokenType::SEMICOLON, "Expected ';' after property declaration");
                properties.push_back({propName.lexeme, propType.lexeme});
            }
            else
            {
                error("Expected constructor, method, or property");
                synchronize();
            }
        }

        consume(TokenType::RBRACE, "Expected '}' after class body");
        return std::make_unique<ClassDecl>(name.lexeme, baseClass, std::move(properties), std::move(methods), std::move(constructor));
    }

    std::unique_ptr<InterfaceDecl> Parser::interfaceDeclaration()
    {
        consume(TokenType::INTERFACE, "Expected 'interface'");
        Token name = consume(TokenType::IDENTIFIER, "Expected interface name");
        consume(TokenType::LBRACE, "Expected '{' after interface name");

        std::vector<std::pair<std::string, std::string>> methods;

        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            Token methodName = consume(TokenType::IDENTIFIER, "Expected method name");
            consume(TokenType::LPAREN, "Expected '(' after method name");

            std::string signature = methodName.lexeme + "(";
            if (!check(TokenType::RPAREN))
            {
                do
                {
                    Token paramName = consume(TokenType::IDENTIFIER, "Expected parameter name");
                    consume(TokenType::COLON, "Expected ':' after parameter name");
                    Token paramType = advance();
                    signature += paramType.lexeme;
                    if (check(TokenType::COMMA))
                        signature += ", ";
                } while (match(TokenType::COMMA));
            }
            signature += ")";

            consume(TokenType::RPAREN, "Expected ')' after parameters");
            consume(TokenType::ARROW, "Expected '->' after parameters");
            Token returnType = advance();
            signature += " -> " + returnType.lexeme;
            consume(TokenType::SEMICOLON, "Expected ';' after method signature");

            methods.push_back({methodName.lexeme, signature});
        }

        consume(TokenType::RBRACE, "Expected '}' after interface body");
        return std::make_unique<InterfaceDecl>(name.lexeme, std::move(methods));
    }

    std::unique_ptr<TypeDecl> Parser::typeDeclaration()
    {
        consume(TokenType::TYPE, "Expected 'type'");
        Token name = consume(TokenType::IDENTIFIER, "Expected type name");

        std::vector<std::string> typeParams;
        // Generic parameters: type Option<T> = ...
        if (match(TokenType::LESS))
        {
            do
            {
                Token param = consume(TokenType::IDENTIFIER, "Expected type parameter");
                typeParams.push_back(param.lexeme);
            } while (match(TokenType::COMMA));
            consume(TokenType::GREATER, "Expected '>' after type parameters");
        }

        consume(TokenType::EQUAL, "Expected '=' after type name");

        std::vector<std::pair<std::string, std::vector<std::string>>> variants;

        // Parse variants: Some(T) | None
        do
        {
            Token variantName = consume(TokenType::IDENTIFIER, "Expected variant name");
            std::vector<std::string> fields;

            if (match(TokenType::LPAREN))
            {
                if (!check(TokenType::RPAREN))
                {
                    do
                    {
                        Token fieldType = advance();
                        fields.push_back(fieldType.lexeme);
                    } while (match(TokenType::COMMA));
                }
                consume(TokenType::RPAREN, "Expected ')' after variant fields");
            }

            variants.push_back({variantName.lexeme, std::move(fields)});

        } while (match(TokenType::PIPE)); // usando | come separatore

        // Optional semicolon
        match(TokenType::SEMICOLON);

        return std::make_unique<TypeDecl>(name.lexeme, std::move(typeParams), std::move(variants));
    }

} // namespace lpp
