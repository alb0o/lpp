#include "Parser.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace lpp
{

    Parser::Parser(const std::vector<Token> &tokens) : tokens(tokens) {}

    Parser::Parser(const std::vector<Token> &tokens, const std::string &source)
        : tokens(tokens), sourceCode(source)
    {
        // Split source into lines for error reporting
        std::stringstream ss(sourceCode);
        std::string line;
        while (std::getline(ss, line))
        {
            sourceLines.push_back(line);
        }
    }

    std::unique_ptr<Program> Parser::parse()
    {
        // First, check for paradigm pragma at the beginning
        ParadigmMode paradigm = ParadigmMode::NONE;

        if (check(TokenType::PRAGMA))
        {
            Token pragmaToken = advance();
            std::string pragmaContent = pragmaToken.lexeme;

            // Parse "pragma paradigm <mode>"
            // Expected format: "pragma paradigm hybrid" (without the #)
            size_t pragmaPos = pragmaContent.find("pragma");
            if (pragmaPos != std::string::npos)
            {
                size_t paradigmPos = pragmaContent.find("paradigm", pragmaPos);
                if (paradigmPos != std::string::npos)
                {
                    // Extract the mode after "paradigm"
                    size_t modeStart = paradigmPos + 8; // length of "paradigm"
                    while (modeStart < pragmaContent.length() && std::isspace(pragmaContent[modeStart]))
                        modeStart++;

                    size_t modeEnd = modeStart;
                    while (modeEnd < pragmaContent.length() && std::isalnum(pragmaContent[modeEnd]))
                        modeEnd++;

                    std::string mode = pragmaContent.substr(modeStart, modeEnd - modeStart);

                    if (mode == "hybrid")
                        paradigm = ParadigmMode::HYBRID;
                    else if (mode == "functional")
                        paradigm = ParadigmMode::FUNCTIONAL;
                    else if (mode == "imperative")
                        paradigm = ParadigmMode::IMPERATIVE;
                    else if (mode == "oop")
                        paradigm = ParadigmMode::OOP;
                    else if (mode == "golfed")
                        paradigm = ParadigmMode::GOLFED;
                    else
                    {
                        error("Invalid paradigm mode '" + mode + "'. Expected: hybrid, functional, imperative, oop, or golfed");
                    }
                }
                else
                {
                    error("Expected 'paradigm' keyword in pragma directive");
                }
            }
        }

        // If no pragma found, error
        if (paradigm == ParadigmMode::NONE)
        {
            error("Missing paradigm declaration. Add '#pragma paradigm <mode>' at the beginning of the file.\n"
                  "Valid modes: hybrid (recommended), functional, imperative, oop, golfed");
            paradigm = ParadigmMode::HYBRID; // Default fallback for error recovery
        }

        std::vector<std::unique_ptr<Function>> functions;
        std::vector<std::unique_ptr<ClassDecl>> classes;
        std::vector<std::unique_ptr<InterfaceDecl>> interfaces;
        std::vector<std::unique_ptr<TypeDecl>> types;
        std::vector<std::unique_ptr<Statement>> enums;
        std::vector<std::unique_ptr<Statement>> imports;
        std::vector<std::unique_ptr<Statement>> exports;

        while (!isAtEnd())
        {
            if (check(TokenType::IMPORT))
            {
                imports.push_back(importStatement());
            }
            else if (check(TokenType::EXPORT))
            {
                exports.push_back(exportStatement());
            }
            else if (check(TokenType::FN) || check(TokenType::ASYNC))
            {
                functions.push_back(function());
            }
            else if (check(TokenType::AUTOPATTERN))
            {
                // autopattern <ProblemType> <ClassName>
                advance(); // consume 'autopattern'
                Token problemType = consume(TokenType::IDENTIFIER, "Expected problem type after 'autopattern'");
                Token className = consume(TokenType::IDENTIFIER, "Expected class name after problem type");
                consume(TokenType::SEMICOLON, "Expected ';' after autopattern declaration");

                // Create auto-pattern statement and expand it into a class
                auto autoPattern = std::make_unique<AutoPatternStmt>(problemType.lexeme, className.lexeme);

                // Auto-detect pattern based on problem type and generate class
                classes.push_back(expandAutoPattern(std::move(autoPattern)));
            }
            else if (check(TokenType::CLASS) || check(TokenType::AT))
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
            else if (check(TokenType::ENUM))
            {
                enums.push_back(enumDeclaration());
            }
            else
            {
                error("Expected function, class, interface, type, or enum declaration");
                synchronize();
            }
        }

        return std::make_unique<Program>(paradigm, std::move(functions), std::move(classes),
                                         std::move(interfaces), std::move(types), std::move(enums),
                                         std::move(imports), std::move(exports));
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

        // Don't throw - return a synthetic error token instead
        Token errorToken;
        errorToken.type = type;
        errorToken.lexeme = "<missing>";
        errorToken.line = peek().line;
        errorToken.column = peek().column;
        return errorToken;
    }

    void Parser::synchronize()
    {
        panicMode = false;
        advance();

        while (!isAtEnd())
        {
            // Stop at statement boundaries
            if (previous().type == TokenType::SEMICOLON)
                return;

            // Stop at declaration keywords
            switch (peek().type)
            {
            case TokenType::CLASS:
            case TokenType::FN:
            case TokenType::LET:
            case TokenType::CONST:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::RETURN:
            case TokenType::IMPORT:
            case TokenType::EXPORT:
            case TokenType::TYPE:
            case TokenType::ENUM:
            case TokenType::INTERFACE:
                return;
            default:
                break;
            }

            advance();
        }
    }

    void Parser::error(const std::string &message)
    {
        if (panicMode)
            return; // Don't report cascading errors
        panicMode = true;

        Token token = peek();
        std::stringstream errorMsg;

        errorMsg << "Parse error at line " << token.line << ", column " << token.column << ":\n";
        errorMsg << "  " << message << "\n";

        // Show source code context if available
        if (!sourceLines.empty() && token.line > 0 && token.line <= sourceLines.size())
        {
            int lineIdx = token.line - 1;
            std::string line = sourceLines[lineIdx];

            // Show the line with line number
            errorMsg << "\n";
            errorMsg << "  " << token.line << " | " << line << "\n";

            // Show caret pointing to error position
            errorMsg << "    | ";
            for (int i = 0; i < token.column - 1; i++)
            {
                errorMsg << " ";
            }
            errorMsg << "^";

            // Add wavy underline for multi-character tokens
            int tokenLen = token.lexeme.length();
            for (int i = 1; i < tokenLen && i < 20; i++)
            {
                errorMsg << "~";
            }
            errorMsg << "\n";

            // Add helpful hint if possible
            if (message.find("Expected ';'") != std::string::npos)
            {
                errorMsg << "\n  Hint: Did you forget a semicolon?\n";
            }
            else if (message.find("Expected ')'") != std::string::npos)
            {
                errorMsg << "\n  Hint: Make sure all parentheses are balanced.\n";
            }
            else if (message.find("Expected '}'") != std::string::npos)
            {
                errorMsg << "\n  Hint: Make sure all braces are balanced.\n";
            }
            else if (message.find("paradigm") != std::string::npos)
            {
                errorMsg << "\n  Hint: Add '#pragma paradigm hybrid' at the top of your file.\n";
            }
        }

        std::string fullError = errorMsg.str();
        errors.push_back(fullError);
        std::cerr << fullError << std::endl;
    }

    std::unique_ptr<Function> Parser::function()
    {
        // Check for async keyword
        bool isAsync = false;
        if (match(TokenType::ASYNC))
        {
            isAsync = true;
        }

        consume(TokenType::FN, "Expected 'fn'");
        Token name = consume(TokenType::IDENTIFIER, "Expected function name");

        // Parse generics: fn foo<T, U>(x: T) -> U
        std::vector<std::string> genericParams;
        if (match(TokenType::LESS))
        {
            do
            {
                Token genericParam = consume(TokenType::IDENTIFIER, "Expected generic parameter");
                genericParams.push_back(genericParam.lexeme);
            } while (match(TokenType::COMMA));

            consume(TokenType::GREATER, "Expected '>' after generic parameters");
        }

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

        auto body = block(true); // Enable implicit return for function body

        auto func = std::make_unique<Function>(name.lexeme, std::move(parameters),
                                               returnType.lexeme, std::move(body),
                                               hasRestParam, restParamName);
        func->isAsync = isAsync;
        func->genericParams = std::move(genericParams);

        return func;
    }

    std::unique_ptr<Statement> Parser::statement()
    {
        if (match(TokenType::QUANTUM))
            return quantumVarDeclaration();
        if (match(TokenType::LET))
            return varDeclaration();
        if (match(TokenType::IF))
            return ifStatement();
        if (match(TokenType::WHILE))
            return whileStatement();
        if (match(TokenType::FOR))
            return forStatement();
        if (match(TokenType::DO))
            return doWhileStatement();
        if (match(TokenType::SWITCH))
            return switchStatement();
        if (match(TokenType::TRY))
            return tryCatchStatement();
        if (match(TokenType::ENUM))
            return enumDeclaration();
        if (match(TokenType::BREAK))
        {
            consume(TokenType::SEMICOLON, "Expected ';' after 'break'");
            return std::make_unique<BreakStmt>();
        }
        if (match(TokenType::CONTINUE))
        {
            consume(TokenType::SEMICOLON, "Expected ';' after 'continue'");
            return std::make_unique<ContinueStmt>();
        }
        if (match(TokenType::RETURN))
            return returnStatement();
        if (match(TokenType::ARROW_LEFT))
            return returnStatement(); // <- alternative syntax

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
            else if (match(TokenType::PLUS_EQUAL) || match(TokenType::MINUS_EQUAL) ||
                     match(TokenType::STAR_EQUAL) || match(TokenType::SLASH_EQUAL))
            {
                // Compound assignment: x += 5 becomes x = x + 5
                Token name = tokens[saved];
                std::string op = previous().lexeme;
                op = op.substr(0, op.length() - 1); // Remove '=' to get operator

                auto value = expression();
                auto varExpr = std::make_unique<IdentifierExpr>(name.lexeme);
                auto compoundExpr = std::make_unique<BinaryExpr>(std::move(varExpr), op, std::move(value));

                consume(TokenType::SEMICOLON, "Expected ';' after compound assignment");
                return std::make_unique<Assignment>(name.lexeme, std::move(compoundExpr));
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

        // Check for destructuring: let [a, b] = arr or let {x, y} = obj or let (a, b) = tuple
        if (check(TokenType::LBRACKET) || check(TokenType::LBRACE) || check(TokenType::LPAREN))
        {
            bool isArray = check(TokenType::LBRACKET);
            bool isObject = check(TokenType::LBRACE);
            bool isTuple = check(TokenType::LPAREN);

            advance(); // consume [, {, or (

            std::vector<std::string> targets;
            do
            {
                Token target = consume(TokenType::IDENTIFIER, "Expected identifier in destructuring");
                targets.push_back(target.lexeme);
            } while (match(TokenType::COMMA));

            if (isTuple)
            {
                consume(TokenType::RPAREN, "Expected ')' after tuple destructuring");
            }
            else
            {
                consume(isArray ? TokenType::RBRACKET : TokenType::RBRACE,
                        isArray ? "Expected ']' after array destructuring" : "Expected '}' after object destructuring");
            }

            consume(TokenType::EQUAL, "Expected '=' after destructuring pattern");
            auto source = expression();
            consume(TokenType::SEMICOLON, "Expected ';' after destructuring");

            return std::make_unique<DestructuringStmt>(std::move(targets), std::move(source), isArray, isTuple);
        }

        Token name = consume(TokenType::IDENTIFIER, "Expected variable name");

        std::string typeName = "auto"; // Default to auto for type inference
        bool isArrayType = false;
        int arraySize = -1; // -1 = dynamic, >0 = fixed size
        bool isNullable = false;
        std::vector<std::string> unionTypes;

        if (isMutable)
        {
            typeName = "mut " + typeName; // Mark as mutable for borrow checker
        }

        if (match(TokenType::COLON))
        {
            Token type = advance();
            typeName = type.lexeme;

            // Check for array type: int[]
            if (match(TokenType::LBRACKET))
            {
                isArrayType = true;

                // Check for fixed size: int[10]
                if (check(TokenType::NUMBER))
                {
                    Token sizeToken = advance();
                    try
                    {
                        arraySize = static_cast<int>(std::stod(sizeToken.lexeme));
                    }
                    catch (const std::exception &e)
                    {
                        error("Invalid array size: " + sizeToken.lexeme);
                        arraySize = 0;
                    }
                }

                consume(TokenType::RBRACKET, "Expected ']' after array type");
            }

            // Check for nullable: int?
            if (match(TokenType::QUESTION))
            {
                isNullable = true;
            }

            // Check for union types: int | string | bool
            if (match(TokenType::PIPE))
            {
                unionTypes.push_back(typeName);
                do
                {
                    Token unionType = advance();
                    unionTypes.push_back(unionType.lexeme);
                } while (match(TokenType::PIPE));
            }

            if (isMutable && typeName.find("mut") == std::string::npos)
            {
                typeName = "mut " + typeName;
            }
        }

        std::unique_ptr<Expression> initializer = nullptr;
        if (match(TokenType::EQUAL))
        {
            initializer = expression();
        }

        consume(TokenType::SEMICOLON, "Expected ';' after variable declaration");

        auto varDecl = std::make_unique<VarDecl>(name.lexeme, typeName, std::move(initializer));
        varDecl->isArrayType = isArrayType;
        varDecl->arraySize = arraySize;
        varDecl->isNullable = isNullable;
        varDecl->unionTypes = std::move(unionTypes);

        return varDecl;
    }

    std::unique_ptr<Statement> Parser::quantumVarDeclaration()
    {
        // quantum let x = [states] or quantum let x: int = [1,2,3]
        consume(TokenType::LET, "Expected 'let' after 'quantum'");

        Token name = consume(TokenType::IDENTIFIER, "Expected quantum variable name");

        std::string typeName = "auto"; // Default type inference

        // Optional type annotation: quantum let x: int = [...]
        if (match(TokenType::COLON))
        {
            Token type = advance();
            typeName = type.lexeme;
        }

        consume(TokenType::EQUAL, "Expected '=' in quantum variable declaration");

        // Parse states: [state1, state2, ...] or {state: prob, ...}
        std::vector<std::unique_ptr<Expression>> states;
        std::vector<double> probabilities;
        bool hasWeights = false;

        if (match(TokenType::LBRACKET))
        {
            // Array of states: [1, 2, 3, 4, 5]
            if (!check(TokenType::RBRACKET))
            {
                do
                {
                    states.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACKET, "Expected ']' after quantum states");
        }
        else if (match(TokenType::LBRACE))
        {
            // Weighted states: {value1: prob1, value2: prob2, ...}
            hasWeights = true;

            if (!check(TokenType::RBRACE))
            {
                do
                {
                    // Parse key (state value)
                    auto stateExpr = expression();
                    states.push_back(std::move(stateExpr));

                    consume(TokenType::COLON, "Expected ':' after quantum state value");

                    // Parse probability
                    Token probToken = consume(TokenType::NUMBER, "Expected probability (number) after ':'");
                    try
                    {
                        probabilities.push_back(std::stod(probToken.lexeme));
                    }
                    catch (const std::exception &e)
                    {
                        error("Invalid probability value: " + probToken.lexeme);
                        probabilities.push_back(0.0);
                    }

                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACE, "Expected '}' after weighted quantum states");
        }
        else
        {
            error("Expected '[' or '{' for quantum variable initialization");
        }

        consume(TokenType::SEMICOLON, "Expected ';' after quantum variable declaration");

        if (hasWeights)
        {
            return std::make_unique<QuantumVarDecl>(name.lexeme, typeName,
                                                    std::move(states), std::move(probabilities));
        }
        else
        {
            return std::make_unique<QuantumVarDecl>(name.lexeme, typeName, std::move(states));
        }
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

    std::unique_ptr<Statement> Parser::switchStatement()
    {
        consume(TokenType::LPAREN, "Expected '(' after 'switch'");
        auto condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after switch condition");
        consume(TokenType::LBRACE, "Expected '{' after switch condition");

        std::vector<CaseClause> cases;

        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            if (match(TokenType::CASE))
            {
                auto caseValue = expression();
                consume(TokenType::COLON, "Expected ':' after case value");

                std::vector<std::unique_ptr<Statement>> caseStmts;
                while (!check(TokenType::CASE) && !check(TokenType::DEFAULT) && !check(TokenType::RBRACE) && !isAtEnd())
                {
                    caseStmts.push_back(statement());
                }

                cases.emplace_back(std::move(caseValue), std::move(caseStmts), false);
            }
            else if (match(TokenType::DEFAULT))
            {
                consume(TokenType::COLON, "Expected ':' after 'default'");

                std::vector<std::unique_ptr<Statement>> defaultStmts;
                while (!check(TokenType::CASE) && !check(TokenType::DEFAULT) && !check(TokenType::RBRACE) && !isAtEnd())
                {
                    defaultStmts.push_back(statement());
                }

                cases.emplace_back(nullptr, std::move(defaultStmts), true);
            }
            else
            {
                error("Expected 'case' or 'default' in switch statement");
                advance();
            }
        }

        consume(TokenType::RBRACE, "Expected '}' after switch cases");

        return std::make_unique<SwitchStmt>(std::move(condition), std::move(cases));
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

    std::vector<std::unique_ptr<Statement>> Parser::block(bool enableImplicitReturn)
    {
        consume(TokenType::LBRACE, "Expected '{'");

        std::vector<std::unique_ptr<Statement>> statements;
        while (!check(TokenType::RBRACE) && !isAtEnd())
        {
            statements.push_back(statement());
        }

        consume(TokenType::RBRACE, "Expected '}'");

        // Handle implicit return: if last statement is ExprStmt, convert to ReturnStmt
        if (enableImplicitReturn && !statements.empty())
        {
            auto *lastStmt = statements.back().get();
            if (auto *exprStmt = dynamic_cast<ExprStmt *>(lastStmt))
            {
                // Last statement is an expression - transform to return
                auto expr = std::move(exprStmt->expression);
                statements.pop_back();
                statements.push_back(std::make_unique<ReturnStmt>(std::move(expr)));
            }
        }

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

        // Check for lambda: x -> expr or x => expr or (params) -> expr or (params) => expr
        size_t saved = current;
        if (check(TokenType::IDENTIFIER))
        {
            advance(); // consume identifier
            if (match(TokenType::ARROW) || match(TokenType::FAT_ARROW))
            {
                // Single param lambda with -> or =>
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

                if (match(TokenType::RPAREN) && (match(TokenType::ARROW) || match(TokenType::FAT_ARROW)))
                {
                    // It's a lambda! Both -> and => are accepted
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

        // Iterate-while: start !! condition $ stepFn
        if (match(TokenType::BANG_BANG))
        {
            auto condition = term();
            if (match(TokenType::DOLLAR))
            {
                auto stepFn = term();
                return std::make_unique<IterateWhileExpr>(std::move(expr), std::move(condition), std::move(stepFn));
            }
            // If no $, treat as error or just !!
            error("Expected '$' after condition in iterate-while expression");
        }

        // Auto-iterate: start !!< limit or start !!> limit
        if (match(TokenType::BANG_BANG_LESS))
        {
            auto limit = term();
            return std::make_unique<AutoIterateExpr>(std::move(expr), std::move(limit), true);
        }
        if (match(TokenType::BANG_BANG_GREATER))
        {
            auto limit = term();
            return std::make_unique<AutoIterateExpr>(std::move(expr), std::move(limit), false);
        }

        // Iterate-step: start ~> stepFn !! condition
        if (match(TokenType::TILDE_GT))
        {
            auto stepFn = term();
            if (match(TokenType::BANG_BANG))
            {
                auto condition = term();
                return std::make_unique<IterateStepExpr>(std::move(expr), std::move(stepFn), std::move(condition));
            }
            error("Expected '!!' after step function in iterate-step expression");
        }

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
        auto expr = cast();

        while (match(TokenType::STAR) || match(TokenType::SLASH) || match(TokenType::PERCENT))
        {
            std::string op = previous().lexeme;
            auto right = cast();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::cast()
    {
        auto expr = unary();

        // Cast expression: x as int
        if (match(TokenType::AS))
        {
            Token targetType = advance();
            return std::make_unique<CastExpr>(std::move(expr), targetType.lexeme);
        }

        return expr;
    }

    std::unique_ptr<Expression> Parser::unary()
    {
        // Await expression
        if (match(TokenType::AWAIT))
        {
            auto expr = unary();
            return std::make_unique<AwaitExpr>(std::move(expr));
        }

        // Throw expression (can be used as expression in some contexts)
        if (match(TokenType::THROW))
        {
            auto expr = unary();
            return std::make_unique<ThrowExpr>(std::move(expr));
        }

        // Prefix increment/decrement: ++i, --i
        if (match(TokenType::PLUS_PLUS) || match(TokenType::MINUS_MINUS))
        {
            std::string op = previous().lexeme;
            auto operand = unary();
            return std::make_unique<UnaryExpr>(op, std::move(operand));
        }

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
                // obj.prop or obj.method() or quantumVar.observe()
                Token propName = peek(); // initialize with current token

                // Allow quantum keywords as method names
                if (check(TokenType::OBSERVE))
                {
                    propName = advance();
                }
                else if (check(TokenType::ENTANGLE))
                {
                    propName = advance();
                }
                else
                {
                    propName = consume(TokenType::IDENTIFIER, "Expected property name after '.'");
                } // Check for quantum methods: observe, map, reset
                if (dynamic_cast<IdentifierExpr *>(expr.get()))
                {
                    std::string varName = dynamic_cast<IdentifierExpr *>(expr.get())->name;
                    std::string method = propName.lexeme;

                    if ((method == "observe" || method == "reset" || method == "map") && check(TokenType::LPAREN))
                    {
                        advance(); // consume '('

                        std::vector<std::unique_ptr<Expression>> args;
                        if (!check(TokenType::RPAREN))
                        {
                            do
                            {
                                args.push_back(expression());
                            } while (match(TokenType::COMMA));
                        }

                        consume(TokenType::RPAREN, "Expected ')' after quantum method call");
                        expr = std::make_unique<QuantumMethodCall>(varName, method, std::move(args));
                        continue;
                    }
                }

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
            else if (check(TokenType::LESS) && dynamic_cast<IdentifierExpr *>(expr.get()))
            {
                // Potential generic function call: foo<int>(x)
                // Need to distinguish from foo < bar
                size_t saved = current;
                advance(); // consume '<'

                bool isGenericCall = false;
                std::vector<std::string> typeArgs;

                // Try to parse as generic type arguments
                if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_FLOAT) ||
                    check(TokenType::TYPE_STRING) || check(TokenType::TYPE_BOOL) ||
                    check(TokenType::IDENTIFIER))
                {
                    do
                    {
                        if (check(TokenType::TYPE_INT) || check(TokenType::TYPE_FLOAT) ||
                            check(TokenType::TYPE_STRING) || check(TokenType::TYPE_BOOL) ||
                            check(TokenType::IDENTIFIER))
                        {
                            Token typeArg = advance();
                            typeArgs.push_back(typeArg.lexeme);
                        }
                        else
                        {
                            break;
                        }
                    } while (match(TokenType::COMMA));

                    if (match(TokenType::GREATER) && check(TokenType::LPAREN))
                    {
                        // It's a generic call: foo<int, string>(...)
                        isGenericCall = true;
                    }
                }

                if (isGenericCall)
                {
                    // Parse function call with generic args
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

                    consume(TokenType::RPAREN, "Expected ')' after generic call arguments");

                    // For now, transpile generic calls by ignoring type args (C++ will infer)
                    expr = std::make_unique<CallExpr>(functionName, std::move(arguments));
                }
                else
                {
                    // Not a generic call, reset and exit to let comparison() handle LESS
                    current = saved;
                    break; // Exit the call() loop so comparison can parse < operator
                }
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
            else if (check(TokenType::PLUS_PLUS) || check(TokenType::MINUS_MINUS))
            {
                // Postfix increment/decrement: i++, i--
                std::string op = advance().lexeme;
                expr = std::make_unique<PostfixExpr>(std::move(expr), op);
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
        // Entangle function: entangle(quantumVar, transformFn)
        if (match(TokenType::ENTANGLE))
        {
            consume(TokenType::LPAREN, "Expected '(' after 'entangle'");

            Token varName = consume(TokenType::IDENTIFIER, "Expected quantum variable name");
            consume(TokenType::COMMA, "Expected ',' after quantum variable");

            auto transformFn = expression();

            consume(TokenType::RPAREN, "Expected ')' after entangle arguments");

            std::vector<std::unique_ptr<Expression>> args;
            args.push_back(std::move(transformFn));

            return std::make_unique<QuantumMethodCall>(varName.lexeme, "entangle", std::move(args));
        }

        if (match(TokenType::NUMBER))
        {
            try
            {
                return std::make_unique<NumberExpr>(std::stod(previous().lexeme));
            }
            catch (const std::exception &e)
            {
                error("Invalid number format: " + previous().lexeme);
                return std::make_unique<NumberExpr>(0.0);
            }
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

        // Grouping or Tuple: (expr) or (1, 2, 3)
        if (match(TokenType::LPAREN))
        {
            // Empty tuple: ()
            if (check(TokenType::RPAREN))
            {
                advance();
                return std::make_unique<TupleExpr>(std::vector<std::unique_ptr<Expression>>());
            }

            auto firstExpr = expression();

            // Check if it's a tuple (has comma) or just grouping
            if (match(TokenType::COMMA))
            {
                // It's a tuple: (expr1, expr2, ...)
                std::vector<std::unique_ptr<Expression>> elements;
                elements.push_back(std::move(firstExpr));

                do
                {
                    if (check(TokenType::RPAREN))
                        break; // trailing comma
                    elements.push_back(expression());
                } while (match(TokenType::COMMA));

                consume(TokenType::RPAREN, "Expected ')' after tuple elements");
                return std::make_unique<TupleExpr>(std::move(elements));
            }
            else
            {
                // Just grouping: (expr)
                consume(TokenType::RPAREN, "Expected ')' after expression");
                return firstExpr;
            }
        }

        error("Expected expression");
        throw std::runtime_error("Expected expression");
    }

    std::unique_ptr<ClassDecl> Parser::classDeclaration()
    {
        // Check for @pattern directive
        std::string designPattern;
        if (check(TokenType::AT))
        {
            advance(); // consume @
            Token patternKeyword = consume(TokenType::IDENTIFIER, "Expected 'pattern' after '@'");
            if (patternKeyword.lexeme == "pattern")
            {
                Token patternName = consume(TokenType::IDENTIFIER, "Expected pattern name after '@pattern'");
                designPattern = patternName.lexeme;
            }
        }

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
            // Property with let/const
            else if (match(TokenType::LET) || match(TokenType::CONST))
            {
                Token propName = consume(TokenType::IDENTIFIER, "Expected property name");
                consume(TokenType::COLON, "Expected ':' after property name");
                Token propType = advance();

                // Optional initialization
                if (match(TokenType::EQUAL))
                {
                    // Skip initialization value for now (will be in constructor)
                    expression();
                }

                consume(TokenType::SEMICOLON, "Expected ';' after property declaration");
                properties.push_back({propName.lexeme, propType.lexeme});
            }
            // Property without let/const (legacy)
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
        auto classDecl = std::make_unique<ClassDecl>(name.lexeme, baseClass, std::move(properties), std::move(methods), std::move(constructor));
        classDecl->designPattern = designPattern;
        return classDecl;
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

    // NEW PARSER METHODS - For loop
    std::unique_ptr<Statement> Parser::forStatement()
    {
        consume(TokenType::LPAREN, "Expected '(' after 'for'");

        // Check for for-in syntax: for (let var in array) or for (var in array)
        size_t saved = current;
        bool hasLet = match(TokenType::LET);

        if (check(TokenType::IDENTIFIER))
        {
            Token var = advance();

            if (match(TokenType::IN) || match(TokenType::OF))
            {
                // For-in loop
                auto iterable = expression();
                consume(TokenType::RPAREN, "Expected ')' after for-in");
                auto body = block();
                return std::make_unique<ForInStmt>(var.lexeme, std::move(iterable), std::move(body));
            }
            else
            {
                // Reset and parse as classic for
                current = saved;
            }
        }
        else
        {
            // Reset if no identifier found
            current = saved;
        }

        // Classic for loop: for (init; cond; inc)
        std::unique_ptr<Statement> initializer = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            if (match(TokenType::LET))
            {
                initializer = varDeclaration();
            }
            else
            {
                initializer = expressionStatement();
            }
        }
        else
        {
            advance(); // consume semicolon
        }

        std::unique_ptr<Expression> condition = nullptr;
        if (!check(TokenType::SEMICOLON))
        {
            condition = expression();
        }
        consume(TokenType::SEMICOLON, "Expected ';' after for condition");

        std::unique_ptr<Expression> increment = nullptr;
        if (!check(TokenType::RPAREN))
        {
            increment = expression();
        }
        consume(TokenType::RPAREN, "Expected ')' after for clauses");

        auto body = block();
        return std::make_unique<ForStmt>(std::move(initializer), std::move(condition),
                                         std::move(increment), std::move(body));
    }

    // NEW PARSER METHODS - Do-while loop
    std::unique_ptr<Statement> Parser::doWhileStatement()
    {
        auto body = block();
        consume(TokenType::WHILE, "Expected 'while' after do block");
        consume(TokenType::LPAREN, "Expected '(' after 'while'");
        auto condition = expression();
        consume(TokenType::RPAREN, "Expected ')' after condition");
        consume(TokenType::SEMICOLON, "Expected ';' after do-while");
        return std::make_unique<DoWhileStmt>(std::move(body), std::move(condition));
    }

    // NEW PARSER METHODS - Try-catch
    std::unique_ptr<Statement> Parser::tryCatchStatement()
    {
        auto tryBlock = block();

        std::string catchVar;
        std::vector<std::unique_ptr<Statement>> catchBlock;
        std::vector<std::unique_ptr<Statement>> finallyBlock;

        if (match(TokenType::CATCH))
        {
            if (match(TokenType::LPAREN))
            {
                Token var = consume(TokenType::IDENTIFIER, "Expected variable name in catch");
                catchVar = var.lexeme;
                consume(TokenType::RPAREN, "Expected ')' after catch variable");
            }
            catchBlock = block();
        }

        if (match(TokenType::FINALLY))
        {
            finallyBlock = block();
        }

        return std::make_unique<TryCatchStmt>(std::move(tryBlock), catchVar,
                                              std::move(catchBlock), std::move(finallyBlock));
    }

    // NEW PARSER METHODS - Enum declaration
    std::unique_ptr<Statement> Parser::enumDeclaration()
    {
        consume(TokenType::ENUM, "Expected 'enum'");
        Token name = consume(TokenType::IDENTIFIER, "Expected enum name");
        consume(TokenType::LBRACE, "Expected '{' after enum name");

        std::vector<std::pair<std::string, int>> values;
        int currentValue = 0;

        if (!check(TokenType::RBRACE))
        {
            do
            {
                Token valueName = consume(TokenType::IDENTIFIER, "Expected enum value name");
                int value = currentValue++;

                if (match(TokenType::EQUAL))
                {
                    Token num = consume(TokenType::NUMBER, "Expected number after '='");
                    value = static_cast<int>(std::stod(num.lexeme));
                    currentValue = value + 1;
                }

                values.push_back({valueName.lexeme, value});

            } while (match(TokenType::COMMA) && !check(TokenType::RBRACE));
        }

        consume(TokenType::RBRACE, "Expected '}' after enum values");
        return std::make_unique<EnumDecl>(name.lexeme, std::move(values));
    }

    // Import statement: import { foo, bar } from "module" or import "module"
    std::unique_ptr<Statement> Parser::importStatement()
    {
        consume(TokenType::IMPORT, "Expected 'import'");

        std::vector<std::string> imports;
        bool importAll = false;

        // import { a, b } from "module" or import "module"
        if (match(TokenType::LBRACE))
        {
            do
            {
                Token name = consume(TokenType::IDENTIFIER, "Expected import name");
                imports.push_back(name.lexeme);
            } while (match(TokenType::COMMA));

            consume(TokenType::RBRACE, "Expected '}' after import list");
            consume(TokenType::FROM, "Expected 'from' after import list");
        }

        Token module = consume(TokenType::STRING, "Expected module path");
        std::string modulePath = module.lexeme;
        // Remove quotes
        if (modulePath.length() >= 2 && modulePath.front() == '\"' && modulePath.back() == '\"')
        {
            modulePath = modulePath.substr(1, modulePath.length() - 2);
        }

        return std::make_unique<ImportStmt>(std::move(imports), modulePath, importAll);
    }

    // Export statement: export fn foo() { } or export class Bar { }
    std::unique_ptr<Statement> Parser::exportStatement()
    {
        consume(TokenType::EXPORT, "Expected 'export'");

        // Parse the declaration to export
        std::unique_ptr<Statement> decl = nullptr;

        if (check(TokenType::FN) || check(TokenType::ASYNC))
        {
            decl = std::make_unique<ExprStmt>(std::make_unique<IdentifierExpr>("exported_function"));
        }
        else if (check(TokenType::CLASS))
        {
            decl = std::make_unique<ExprStmt>(std::make_unique<IdentifierExpr>("exported_class"));
        }

        return std::make_unique<ExportStmt>(std::move(decl));
    }

    // Auto-pattern expansion: intelligently generates design pattern based on problem keywords
    std::unique_ptr<ClassDecl> Parser::expandAutoPattern(std::unique_ptr<AutoPatternStmt> autoPattern)
    {
        std::string problem = autoPattern->problemType;
        std::string className = autoPattern->className;
        std::string pattern;

        // ==================== CREATIONAL PATTERNS (5) ====================
        if (problem.find("Singleton") != std::string::npos ||
            problem.find("Config") != std::string::npos ||
            problem.find("Settings") != std::string::npos ||
            problem.find("Global") != std::string::npos)
        {
            pattern = "Singleton";
        }
        else if (problem.find("AbstractFactory") != std::string::npos ||
                 problem.find("FamilyOf") != std::string::npos)
        {
            pattern = "AbstractFactory";
        }
        else if (problem.find("Builder") != std::string::npos ||
                 problem.find("Construct") != std::string::npos ||
                 problem.find("StepByStep") != std::string::npos)
        {
            pattern = "Builder";
        }
        else if (problem.find("Factory") != std::string::npos ||
                 problem.find("Create") != std::string::npos ||
                 problem.find("Instantiate") != std::string::npos)
        {
            pattern = "Factory";
        }
        else if (problem.find("Prototype") != std::string::npos ||
                 problem.find("Clone") != std::string::npos ||
                 problem.find("Copy") != std::string::npos)
        {
            pattern = "Prototype";
        }
        // ==================== STRUCTURAL PATTERNS (7) ====================
        else if (problem.find("Adapter") != std::string::npos ||
                 problem.find("Wrapper") != std::string::npos ||
                 problem.find("Convert") != std::string::npos)
        {
            pattern = "Adapter";
        }
        else if (problem.find("Bridge") != std::string::npos ||
                 problem.find("Decouple") != std::string::npos ||
                 problem.find("Abstraction") != std::string::npos)
        {
            pattern = "Bridge";
        }
        else if (problem.find("Composite") != std::string::npos ||
                 problem.find("Tree") != std::string::npos ||
                 problem.find("Hierarchy") != std::string::npos)
        {
            pattern = "Composite";
        }
        else if (problem.find("Decorator") != std::string::npos ||
                 problem.find("Enhance") != std::string::npos ||
                 problem.find("Extend") != std::string::npos)
        {
            pattern = "Decorator";
        }
        else if (problem.find("Facade") != std::string::npos ||
                 problem.find("Simplify") != std::string::npos ||
                 problem.find("Unified") != std::string::npos)
        {
            pattern = "Facade";
        }
        else if (problem.find("Flyweight") != std::string::npos ||
                 problem.find("Shared") != std::string::npos ||
                 problem.find("Pool") != std::string::npos)
        {
            pattern = "Flyweight";
        }
        else if (problem.find("Proxy") != std::string::npos ||
                 problem.find("Surrogate") != std::string::npos ||
                 problem.find("Placeholder") != std::string::npos)
        {
            pattern = "Proxy";
        }
        // ==================== BEHAVIORAL PATTERNS (11) ====================
        else if (problem.find("Chain") != std::string::npos ||
                 problem.find("Handler") != std::string::npos ||
                 problem.find("Request") != std::string::npos)
        {
            pattern = "ChainOfResponsibility";
        }
        else if (problem.find("Command") != std::string::npos ||
                 problem.find("Action") != std::string::npos ||
                 problem.find("Execute") != std::string::npos)
        {
            pattern = "Command";
        }
        else if (problem.find("Iterator") != std::string::npos ||
                 problem.find("Traverse") != std::string::npos ||
                 problem.find("Collection") != std::string::npos)
        {
            pattern = "Iterator";
        }
        else if (problem.find("Mediator") != std::string::npos ||
                 problem.find("Coordinate") != std::string::npos ||
                 problem.find("Central") != std::string::npos)
        {
            pattern = "Mediator";
        }
        else if (problem.find("Memento") != std::string::npos ||
                 problem.find("Snapshot") != std::string::npos ||
                 problem.find("Undo") != std::string::npos)
        {
            pattern = "Memento";
        }
        else if (problem.find("Observer") != std::string::npos ||
                 problem.find("Notify") != std::string::npos ||
                 problem.find("Event") != std::string::npos ||
                 problem.find("Subscribe") != std::string::npos)
        {
            pattern = "Observer";
        }
        else if (problem.find("State") != std::string::npos ||
                 problem.find("Context") != std::string::npos ||
                 problem.find("Mode") != std::string::npos)
        {
            pattern = "State";
        }
        else if (problem.find("Strategy") != std::string::npos ||
                 problem.find("Algorithm") != std::string::npos ||
                 problem.find("Policy") != std::string::npos)
        {
            pattern = "Strategy";
        }
        else if (problem.find("Template") != std::string::npos ||
                 problem.find("Skeleton") != std::string::npos ||
                 problem.find("Hook") != std::string::npos)
        {
            pattern = "TemplateMethod";
        }
        else if (problem.find("Visitor") != std::string::npos ||
                 problem.find("Operation") != std::string::npos ||
                 problem.find("Element") != std::string::npos)
        {
            pattern = "Visitor";
        }
        else if (problem.find("Interpreter") != std::string::npos ||
                 problem.find("Grammar") != std::string::npos ||
                 problem.find("Language") != std::string::npos)
        {
            pattern = "Interpreter";
        }
        // ==================== FUNCTIONAL PATTERNS (20) ====================
        else if (problem.find("Monad") != std::string::npos ||
                 problem.find("Compose") != std::string::npos ||
                 problem.find("Bind") != std::string::npos ||
                 problem.find("FlatMap") != std::string::npos)
        {
            pattern = "Monad";
        }
        else if (problem.find("Functor") != std::string::npos ||
                 problem.find("Mappable") != std::string::npos ||
                 problem.find("Transform") != std::string::npos)
        {
            pattern = "Functor";
        }
        else if (problem.find("Applicative") != std::string::npos ||
                 problem.find("Apply") != std::string::npos ||
                 problem.find("Lift") != std::string::npos)
        {
            pattern = "Applicative";
        }
        else if (problem.find("Monoid") != std::string::npos ||
                 problem.find("Combine") != std::string::npos ||
                 problem.find("Concat") != std::string::npos ||
                 problem.find("Semigroup") != std::string::npos)
        {
            pattern = "Monoid";
        }
        else if (problem.find("Foldable") != std::string::npos ||
                 problem.find("Reduce") != std::string::npos ||
                 problem.find("Fold") != std::string::npos)
        {
            pattern = "Foldable";
        }
        else if (problem.find("Traversable") != std::string::npos ||
                 problem.find("Traverse") != std::string::npos ||
                 problem.find("Sequence") != std::string::npos)
        {
            pattern = "Traversable";
        }
        else if (problem.find("Lens") != std::string::npos ||
                 problem.find("Optic") != std::string::npos ||
                 problem.find("Focus") != std::string::npos ||
                 problem.find("View") != std::string::npos)
        {
            pattern = "Lens";
        }
        else if (problem.find("Either") != std::string::npos ||
                 problem.find("Result") != std::string::npos ||
                 problem.find("LeftRight") != std::string::npos)
        {
            pattern = "Either";
        }
        else if (problem.find("Maybe") != std::string::npos ||
                 problem.find("Option") != std::string::npos ||
                 problem.find("Optional") != std::string::npos ||
                 problem.find("Some") != std::string::npos)
        {
            pattern = "Maybe";
        }
        else if (problem.find("StateMonad") != std::string::npos ||
                 problem.find("StateFn") != std::string::npos ||
                 problem.find("ImmutableState") != std::string::npos)
        {
            pattern = "StateMonad";
        }
        else if (problem.find("Reader") != std::string::npos ||
                 problem.find("Environment") != std::string::npos ||
                 problem.find("Dependency") != std::string::npos ||
                 problem.find("Config") != std::string::npos)
        {
            pattern = "Reader";
        }
        else if (problem.find("Writer") != std::string::npos ||
                 problem.find("Logger") != std::string::npos ||
                 problem.find("Log") != std::string::npos)
        {
            pattern = "Writer";
        }
        else if (problem.find("IO") != std::string::npos ||
                 problem.find("Effect") != std::string::npos ||
                 problem.find("SideEffect") != std::string::npos)
        {
            pattern = "IO";
        }
        else if (problem.find("Free") != std::string::npos ||
                 problem.find("AST") != std::string::npos ||
                 problem.find("Program") != std::string::npos)
        {
            pattern = "Free";
        }
        else if (problem.find("Continuation") != std::string::npos ||
                 problem.find("CPS") != std::string::npos ||
                 problem.find("Callback") != std::string::npos)
        {
            pattern = "Continuation";
        }
        else if (problem.find("Comonad") != std::string::npos ||
                 problem.find("Extract") != std::string::npos ||
                 problem.find("Extend") != std::string::npos)
        {
            pattern = "Comonad";
        }
        else if (problem.find("Zipper") != std::string::npos ||
                 problem.find("Cursor") != std::string::npos ||
                 problem.find("Navigate") != std::string::npos)
        {
            pattern = "Zipper";
        }
        else if (problem.find("Church") != std::string::npos ||
                 problem.find("Encoding") != std::string::npos ||
                 problem.find("Lambda") != std::string::npos)
        {
            pattern = "Church";
        }
        else if (problem.find("Algebraic") != std::string::npos ||
                 problem.find("Effect") != std::string::npos ||
                 problem.find("Handler") != std::string::npos)
        {
            pattern = "AlgebraicEffect";
        }
        else if (problem.find("Catamorphism") != std::string::npos ||
                 problem.find("Anamorphism") != std::string::npos ||
                 problem.find("Unfold") != std::string::npos ||
                 problem.find("Recursion") != std::string::npos)
        {
            pattern = "Morphism";
        }
        else
        {
            // Default to Factory if unclear
            pattern = "Factory";
        }

        autoPattern->patternType = pattern;

        // Generate pattern-specific structure
        std::vector<std::pair<std::string, std::string>> properties;
        std::vector<std::unique_ptr<Function>> methods;
        std::unique_ptr<Function> constructor = nullptr;

        // ==================== CREATIONAL PATTERNS ====================

        if (pattern == "Singleton")
        {
            properties.push_back({"instance", className + "*"});
            std::vector<std::unique_ptr<Statement>> getInstanceBody;
            auto returnStmt = std::make_unique<ReturnStmt>(std::make_unique<IdentifierExpr>("instance"));
            getInstanceBody.push_back(std::move(returnStmt));
            auto getInstance = std::make_unique<Function>("getInstance",
                                                          std::vector<std::pair<std::string, std::string>>(),
                                                          className + "*", std::move(getInstanceBody));
            methods.push_back(std::move(getInstance));
        }
        else if (pattern == "Factory")
        {
            properties.push_back({"products", "map<string, Product*>"});
            std::vector<std::pair<std::string, std::string>> createParams;
            createParams.push_back({"type", "string"});
            std::vector<std::unique_ptr<Statement>> createBody;
            auto returnStmt = std::make_unique<ReturnStmt>(nullptr);
            createBody.push_back(std::move(returnStmt));
            auto createMethod = std::make_unique<Function>("create",
                                                           std::move(createParams), "Product*", std::move(createBody));
            methods.push_back(std::move(createMethod));
        }
        else if (pattern == "AbstractFactory")
        {
            properties.push_back({"factories", "map<string, Factory*>"});
            std::vector<std::unique_ptr<Statement>> createProductBody;
            auto createProduct = std::make_unique<Function>("createProductA",
                                                            std::vector<std::pair<std::string, std::string>>(),
                                                            "ProductA*", std::move(createProductBody));
            methods.push_back(std::move(createProduct));
            std::vector<std::unique_ptr<Statement>> createProductBBody;
            auto createProductB = std::make_unique<Function>("createProductB",
                                                             std::vector<std::pair<std::string, std::string>>(),
                                                             "ProductB*", std::move(createProductBBody));
            methods.push_back(std::move(createProductB));
        }
        else if (pattern == "Builder")
        {
            properties.push_back({"product", "Product*"});
            std::vector<std::unique_ptr<Statement>> buildPartBody;
            auto buildPartA = std::make_unique<Function>("buildPartA",
                                                         std::vector<std::pair<std::string, std::string>>(),
                                                         "void", std::move(buildPartBody));
            methods.push_back(std::move(buildPartA));
            std::vector<std::unique_ptr<Statement>> getResultBody;
            auto getResult = std::make_unique<Function>("getResult",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "Product*", std::move(getResultBody));
            methods.push_back(std::move(getResult));
        }
        else if (pattern == "Prototype")
        {
            std::vector<std::unique_ptr<Statement>> cloneBody;
            auto clone = std::make_unique<Function>("clone",
                                                    std::vector<std::pair<std::string, std::string>>(),
                                                    className + "*", std::move(cloneBody));
            methods.push_back(std::move(clone));
        }

        // ==================== STRUCTURAL PATTERNS ====================

        else if (pattern == "Adapter")
        {
            properties.push_back({"adaptee", "Adaptee*"});
            std::vector<std::unique_ptr<Statement>> requestBody;
            auto request = std::make_unique<Function>("request",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "void", std::move(requestBody));
            methods.push_back(std::move(request));
        }
        else if (pattern == "Bridge")
        {
            properties.push_back({"implementor", "Implementor*"});
            std::vector<std::unique_ptr<Statement>> operationBody;
            auto operation = std::make_unique<Function>("operation",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "void", std::move(operationBody));
            methods.push_back(std::move(operation));
        }
        else if (pattern == "Composite")
        {
            properties.push_back({"children", "vector<Component*>"});
            std::vector<std::pair<std::string, std::string>> addParams;
            addParams.push_back({"component", "Component*"});
            std::vector<std::unique_ptr<Statement>> addBody;
            auto add = std::make_unique<Function>("add", std::move(addParams), "void", std::move(addBody));
            methods.push_back(std::move(add));
            std::vector<std::unique_ptr<Statement>> operationBody;
            auto operation = std::make_unique<Function>("operation",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "void", std::move(operationBody));
            methods.push_back(std::move(operation));
        }
        else if (pattern == "Decorator")
        {
            properties.push_back({"component", "Component*"});
            std::vector<std::unique_ptr<Statement>> operationBody;
            auto operation = std::make_unique<Function>("operation",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "void", std::move(operationBody));
            methods.push_back(std::move(operation));
        }
        else if (pattern == "Facade")
        {
            properties.push_back({"subsystem1", "Subsystem1*"});
            properties.push_back({"subsystem2", "Subsystem2*"});
            std::vector<std::unique_ptr<Statement>> operationBody;
            auto operation = std::make_unique<Function>("operation",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "void", std::move(operationBody));
            methods.push_back(std::move(operation));
        }
        else if (pattern == "Flyweight")
        {
            properties.push_back({"intrinsicState", "string"});
            properties.push_back({"pool", "map<string, Flyweight*>"});
            std::vector<std::pair<std::string, std::string>> operationParams;
            operationParams.push_back({"extrinsicState", "string"});
            std::vector<std::unique_ptr<Statement>> operationBody;
            auto operation = std::make_unique<Function>("operation", std::move(operationParams),
                                                        "void", std::move(operationBody));
            methods.push_back(std::move(operation));
        }
        else if (pattern == "Proxy")
        {
            properties.push_back({"realSubject", "RealSubject*"});
            std::vector<std::unique_ptr<Statement>> requestBody;
            auto request = std::make_unique<Function>("request",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "void", std::move(requestBody));
            methods.push_back(std::move(request));
        }

        // ==================== BEHAVIORAL PATTERNS ====================

        else if (pattern == "ChainOfResponsibility")
        {
            properties.push_back({"successor", "Handler*"});
            std::vector<std::pair<std::string, std::string>> handleParams;
            handleParams.push_back({"request", "Request*"});
            std::vector<std::unique_ptr<Statement>> handleBody;
            auto handleRequest = std::make_unique<Function>("handleRequest", std::move(handleParams),
                                                            "void", std::move(handleBody));
            methods.push_back(std::move(handleRequest));
        }
        else if (pattern == "Command")
        {
            properties.push_back({"receiver", "Receiver*"});
            std::vector<std::unique_ptr<Statement>> executeBody;
            auto execute = std::make_unique<Function>("execute",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "void", std::move(executeBody));
            methods.push_back(std::move(execute));
            std::vector<std::unique_ptr<Statement>> undoBody;
            auto undo = std::make_unique<Function>("undo",
                                                   std::vector<std::pair<std::string, std::string>>(),
                                                   "void", std::move(undoBody));
            methods.push_back(std::move(undo));
        }
        else if (pattern == "Iterator")
        {
            properties.push_back({"collection", "Collection*"});
            properties.push_back({"current", "int"});
            std::vector<std::unique_ptr<Statement>> nextBody;
            auto next = std::make_unique<Function>("next",
                                                   std::vector<std::pair<std::string, std::string>>(),
                                                   "Item*", std::move(nextBody));
            methods.push_back(std::move(next));
            std::vector<std::unique_ptr<Statement>> hasNextBody;
            auto hasNext = std::make_unique<Function>("hasNext",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "bool", std::move(hasNextBody));
            methods.push_back(std::move(hasNext));
        }
        else if (pattern == "Mediator")
        {
            properties.push_back({"colleagues", "vector<Colleague*>"});
            std::vector<std::pair<std::string, std::string>> notifyParams;
            notifyParams.push_back({"sender", "Colleague*"});
            notifyParams.push_back({"event", "string"});
            std::vector<std::unique_ptr<Statement>> notifyBody;
            auto notify = std::make_unique<Function>("notify", std::move(notifyParams),
                                                     "void", std::move(notifyBody));
            methods.push_back(std::move(notify));
        }
        else if (pattern == "Memento")
        {
            properties.push_back({"state", "string"});
            std::vector<std::unique_ptr<Statement>> saveBody;
            auto save = std::make_unique<Function>("save",
                                                   std::vector<std::pair<std::string, std::string>>(),
                                                   "Memento*", std::move(saveBody));
            methods.push_back(std::move(save));
            std::vector<std::pair<std::string, std::string>> restoreParams;
            restoreParams.push_back({"memento", "Memento*"});
            std::vector<std::unique_ptr<Statement>> restoreBody;
            auto restore = std::make_unique<Function>("restore", std::move(restoreParams),
                                                      "void", std::move(restoreBody));
            methods.push_back(std::move(restore));
        }
        else if (pattern == "Observer")
        {
            properties.push_back({"observers", "vector<Observer*>"});
            std::vector<std::pair<std::string, std::string>> attachParams;
            attachParams.push_back({"observer", "Observer*"});
            std::vector<std::unique_ptr<Statement>> attachBody;
            auto attach = std::make_unique<Function>("attach", std::move(attachParams),
                                                     "void", std::move(attachBody));
            methods.push_back(std::move(attach));
            std::vector<std::unique_ptr<Statement>> notifyBody;
            auto notify = std::make_unique<Function>("notify",
                                                     std::vector<std::pair<std::string, std::string>>(),
                                                     "void", std::move(notifyBody));
            methods.push_back(std::move(notify));
        }
        else if (pattern == "State")
        {
            properties.push_back({"state", "State*"});
            std::vector<std::unique_ptr<Statement>> handleBody;
            auto handle = std::make_unique<Function>("handle",
                                                     std::vector<std::pair<std::string, std::string>>(),
                                                     "void", std::move(handleBody));
            methods.push_back(std::move(handle));
            std::vector<std::pair<std::string, std::string>> setStateParams;
            setStateParams.push_back({"newState", "State*"});
            std::vector<std::unique_ptr<Statement>> setStateBody;
            auto setState = std::make_unique<Function>("setState", std::move(setStateParams),
                                                       "void", std::move(setStateBody));
            methods.push_back(std::move(setState));
        }
        else if (pattern == "Strategy")
        {
            properties.push_back({"strategy", "Strategy*"});
            std::vector<std::unique_ptr<Statement>> executeBody;
            auto execute = std::make_unique<Function>("execute",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "void", std::move(executeBody));
            methods.push_back(std::move(execute));
        }
        else if (pattern == "TemplateMethod")
        {
            std::vector<std::unique_ptr<Statement>> templateMethodBody;
            auto templateMethod = std::make_unique<Function>("templateMethod",
                                                             std::vector<std::pair<std::string, std::string>>(),
                                                             "void", std::move(templateMethodBody));
            methods.push_back(std::move(templateMethod));
            std::vector<std::unique_ptr<Statement>> step1Body;
            auto step1 = std::make_unique<Function>("step1",
                                                    std::vector<std::pair<std::string, std::string>>(),
                                                    "void", std::move(step1Body));
            methods.push_back(std::move(step1));
            std::vector<std::unique_ptr<Statement>> step2Body;
            auto step2 = std::make_unique<Function>("step2",
                                                    std::vector<std::pair<std::string, std::string>>(),
                                                    "void", std::move(step2Body));
            methods.push_back(std::move(step2));
        }
        else if (pattern == "Visitor")
        {
            std::vector<std::pair<std::string, std::string>> visitParams;
            visitParams.push_back({"element", "Element*"});
            std::vector<std::unique_ptr<Statement>> visitBody;
            auto visit = std::make_unique<Function>("visit", std::move(visitParams),
                                                    "void", std::move(visitBody));
            methods.push_back(std::move(visit));
        }
        else if (pattern == "Interpreter")
        {
            properties.push_back({"context", "Context*"});
            std::vector<std::unique_ptr<Statement>> interpretBody;
            auto interpret = std::make_unique<Function>("interpret",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "void", std::move(interpretBody));
            methods.push_back(std::move(interpret));
        }

        // ==================== FUNCTIONAL PATTERNS (20) ====================

        else if (pattern == "Monad")
        {
            // Monad: pure composition with bind/flatMap
            properties.push_back({"value", "T"});

            // bind/flatMap method
            std::vector<std::pair<std::string, std::string>> bindParams;
            bindParams.push_back({"f", "function<Monad<U>(T)>"});
            std::vector<std::unique_ptr<Statement>> bindBody;
            auto bind = std::make_unique<Function>("bind", std::move(bindParams),
                                                   "Monad<U>", std::move(bindBody));
            methods.push_back(std::move(bind));

            // pure/return method (static factory)
            std::vector<std::pair<std::string, std::string>> pureParams;
            pureParams.push_back({"x", "T"});
            std::vector<std::unique_ptr<Statement>> pureBody;
            auto pure = std::make_unique<Function>("pure", std::move(pureParams),
                                                   "Monad<T>", std::move(pureBody));
            methods.push_back(std::move(pure));
        }
        else if (pattern == "Functor")
        {
            // Functor: map transformation
            properties.push_back({"value", "T"});

            // map method
            std::vector<std::pair<std::string, std::string>> mapParams;
            mapParams.push_back({"f", "function<U(T)>"});
            std::vector<std::unique_ptr<Statement>> mapBody;
            auto map = std::make_unique<Function>("map", std::move(mapParams),
                                                  "Functor<U>", std::move(mapBody));
            methods.push_back(std::move(map));
        }
        else if (pattern == "Applicative")
        {
            // Applicative: apply wrapped functions
            properties.push_back({"value", "T"});

            // apply method
            std::vector<std::pair<std::string, std::string>> applyParams;
            applyParams.push_back({"f", "Applicative<function<U(T)>>"});
            std::vector<std::unique_ptr<Statement>> applyBody;
            auto apply = std::make_unique<Function>("apply", std::move(applyParams),
                                                    "Applicative<U>", std::move(applyBody));
            methods.push_back(std::move(apply));

            // pure method
            std::vector<std::pair<std::string, std::string>> pureParams;
            pureParams.push_back({"x", "T"});
            std::vector<std::unique_ptr<Statement>> pureBody;
            auto pure = std::make_unique<Function>("pure", std::move(pureParams),
                                                   "Applicative<T>", std::move(pureBody));
            methods.push_back(std::move(pure));
        }
        else if (pattern == "Monoid")
        {
            // Monoid: associative binary operation with identity
            properties.push_back({"value", "T"});

            // combine/mappend method
            std::vector<std::pair<std::string, std::string>> combineParams;
            combineParams.push_back({"other", "Monoid<T>"});
            std::vector<std::unique_ptr<Statement>> combineBody;
            auto combine = std::make_unique<Function>("combine", std::move(combineParams),
                                                      "Monoid<T>", std::move(combineBody));
            methods.push_back(std::move(combine));

            // empty/mempty method (identity element)
            std::vector<std::unique_ptr<Statement>> emptyBody;
            auto empty = std::make_unique<Function>("empty",
                                                    std::vector<std::pair<std::string, std::string>>(),
                                                    "Monoid<T>", std::move(emptyBody));
            methods.push_back(std::move(empty));
        }
        else if (pattern == "Foldable")
        {
            // Foldable: reduce/fold operations
            properties.push_back({"container", "Container<T>"});

            // foldLeft method
            std::vector<std::pair<std::string, std::string>> foldLeftParams;
            foldLeftParams.push_back({"init", "U"});
            foldLeftParams.push_back({"f", "function<U(U, T)>"});
            std::vector<std::unique_ptr<Statement>> foldLeftBody;
            auto foldLeft = std::make_unique<Function>("foldLeft", std::move(foldLeftParams),
                                                       "U", std::move(foldLeftBody));
            methods.push_back(std::move(foldLeft));

            // foldRight method
            std::vector<std::pair<std::string, std::string>> foldRightParams;
            foldRightParams.push_back({"init", "U"});
            foldRightParams.push_back({"f", "function<U(T, U)>"});
            std::vector<std::unique_ptr<Statement>> foldRightBody;
            auto foldRight = std::make_unique<Function>("foldRight", std::move(foldRightParams),
                                                        "U", std::move(foldRightBody));
            methods.push_back(std::move(foldRight));
        }
        else if (pattern == "Traversable")
        {
            // Traversable: traverse with effects
            properties.push_back({"container", "Container<T>"});

            // traverse method
            std::vector<std::pair<std::string, std::string>> traverseParams;
            traverseParams.push_back({"f", "function<Applicative<U>(T)>"});
            std::vector<std::unique_ptr<Statement>> traverseBody;
            auto traverse = std::make_unique<Function>("traverse", std::move(traverseParams),
                                                       "Applicative<Container<U>>", std::move(traverseBody));
            methods.push_back(std::move(traverse));

            // sequence method
            std::vector<std::unique_ptr<Statement>> sequenceBody;
            auto sequence = std::make_unique<Function>("sequence",
                                                       std::vector<std::pair<std::string, std::string>>(),
                                                       "Applicative<Container<T>>", std::move(sequenceBody));
            methods.push_back(std::move(sequence));
        }
        else if (pattern == "Lens")
        {
            // Lens: functional references for immutable updates
            properties.push_back({"getter", "function<T(S)>"});
            properties.push_back({"setter", "function<S(S, T)>"});

            // view method
            std::vector<std::pair<std::string, std::string>> viewParams;
            viewParams.push_back({"s", "S"});
            std::vector<std::unique_ptr<Statement>> viewBody;
            auto view = std::make_unique<Function>("view", std::move(viewParams),
                                                   "T", std::move(viewBody));
            methods.push_back(std::move(view));

            // set method
            std::vector<std::pair<std::string, std::string>> setParams;
            setParams.push_back({"s", "S"});
            setParams.push_back({"t", "T"});
            std::vector<std::unique_ptr<Statement>> setBody;
            auto set = std::make_unique<Function>("set", std::move(setParams),
                                                  "S", std::move(setBody));
            methods.push_back(std::move(set));

            // over/modify method
            std::vector<std::pair<std::string, std::string>> overParams;
            overParams.push_back({"s", "S"});
            overParams.push_back({"f", "function<T(T)>"});
            std::vector<std::unique_ptr<Statement>> overBody;
            auto over = std::make_unique<Function>("over", std::move(overParams),
                                                   "S", std::move(overBody));
            methods.push_back(std::move(over));
        }
        else if (pattern == "Either")
        {
            // Either: error handling with Left/Right
            properties.push_back({"value", "variant<L, R>"});
            properties.push_back({"isLeft", "bool"});

            // left constructor
            std::vector<std::pair<std::string, std::string>> leftParams;
            leftParams.push_back({"l", "L"});
            std::vector<std::unique_ptr<Statement>> leftBody;
            auto left = std::make_unique<Function>("left", std::move(leftParams),
                                                   "Either<L, R>", std::move(leftBody));
            methods.push_back(std::move(left));

            // right constructor
            std::vector<std::pair<std::string, std::string>> rightParams;
            rightParams.push_back({"r", "R"});
            std::vector<std::unique_ptr<Statement>> rightBody;
            auto right = std::make_unique<Function>("right", std::move(rightParams),
                                                    "Either<L, R>", std::move(rightBody));
            methods.push_back(std::move(right));

            // fold method
            std::vector<std::pair<std::string, std::string>> foldParams;
            foldParams.push_back({"onLeft", "function<T(L)>"});
            foldParams.push_back({"onRight", "function<T(R)>"});
            std::vector<std::unique_ptr<Statement>> foldBody;
            auto fold = std::make_unique<Function>("fold", std::move(foldParams),
                                                   "T", std::move(foldBody));
            methods.push_back(std::move(fold));
        }
        else if (pattern == "Maybe")
        {
            // Maybe/Option: optional values with Nothing/Just
            properties.push_back({"value", "optional<T>"});

            // just constructor
            std::vector<std::pair<std::string, std::string>> justParams;
            justParams.push_back({"x", "T"});
            std::vector<std::unique_ptr<Statement>> justBody;
            auto just = std::make_unique<Function>("just", std::move(justParams),
                                                   "Maybe<T>", std::move(justBody));
            methods.push_back(std::move(just));

            // nothing constructor
            std::vector<std::unique_ptr<Statement>> nothingBody;
            auto nothing = std::make_unique<Function>("nothing",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "Maybe<T>", std::move(nothingBody));
            methods.push_back(std::move(nothing));

            // getOrElse method
            std::vector<std::pair<std::string, std::string>> getOrElseParams;
            getOrElseParams.push_back({"defaultValue", "T"});
            std::vector<std::unique_ptr<Statement>> getOrElseBody;
            auto getOrElse = std::make_unique<Function>("getOrElse", std::move(getOrElseParams),
                                                        "T", std::move(getOrElseBody));
            methods.push_back(std::move(getOrElse));
        }
        else if (pattern == "StateMonad")
        {
            // StateMonad: stateful computation as function
            properties.push_back({"runState", "function<pair<A, S>(S)>"});

            // bind method
            std::vector<std::pair<std::string, std::string>> bindParams;
            bindParams.push_back({"f", "function<State<B, S>(A)>"});
            std::vector<std::unique_ptr<Statement>> bindBody;
            auto bind = std::make_unique<Function>("bind", std::move(bindParams),
                                                   "State<B, S>", std::move(bindBody));
            methods.push_back(std::move(bind));

            // get (read state)
            std::vector<std::unique_ptr<Statement>> getBody;
            auto get = std::make_unique<Function>("get",
                                                  std::vector<std::pair<std::string, std::string>>(),
                                                  "State<S, S>", std::move(getBody));
            methods.push_back(std::move(get));

            // put (write state)
            std::vector<std::pair<std::string, std::string>> putParams;
            putParams.push_back({"newState", "S"});
            std::vector<std::unique_ptr<Statement>> putBody;
            auto put = std::make_unique<Function>("put", std::move(putParams),
                                                  "State<void, S>", std::move(putBody));
            methods.push_back(std::move(put));
        }
        else if (pattern == "Reader")
        {
            // Reader: dependency injection monad
            properties.push_back({"runReader", "function<A(R)>"});

            // ask (read environment)
            std::vector<std::unique_ptr<Statement>> askBody;
            auto ask = std::make_unique<Function>("ask",
                                                  std::vector<std::pair<std::string, std::string>>(),
                                                  "Reader<R, R>", std::move(askBody));
            methods.push_back(std::move(ask));

            // local (modify environment)
            std::vector<std::pair<std::string, std::string>> localParams;
            localParams.push_back({"f", "function<R(R)>"});
            std::vector<std::unique_ptr<Statement>> localBody;
            auto local = std::make_unique<Function>("local", std::move(localParams),
                                                    "Reader<R, A>", std::move(localBody));
            methods.push_back(std::move(local));
        }
        else if (pattern == "Writer")
        {
            // Writer: logging/output accumulation monad
            properties.push_back({"value", "A"});
            properties.push_back({"log", "W"});

            // tell (write to log)
            std::vector<std::pair<std::string, std::string>> tellParams;
            tellParams.push_back({"msg", "W"});
            std::vector<std::unique_ptr<Statement>> tellBody;
            auto tell = std::make_unique<Function>("tell", std::move(tellParams),
                                                   "Writer<void, W>", std::move(tellBody));
            methods.push_back(std::move(tell));

            // listen (access log)
            std::vector<std::unique_ptr<Statement>> listenBody;
            auto listen = std::make_unique<Function>("listen",
                                                     std::vector<std::pair<std::string, std::string>>(),
                                                     "Writer<pair<A, W>, W>", std::move(listenBody));
            methods.push_back(std::move(listen));
        }
        else if (pattern == "IO")
        {
            // IO: side-effect wrapper
            properties.push_back({"action", "function<A()>"});

            // unsafePerformIO (execute side effect)
            std::vector<std::unique_ptr<Statement>> performBody;
            auto perform = std::make_unique<Function>("unsafePerformIO",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "A", std::move(performBody));
            methods.push_back(std::move(perform));

            // bind method
            std::vector<std::pair<std::string, std::string>> bindParams;
            bindParams.push_back({"f", "function<IO<B>(A)>"});
            std::vector<std::unique_ptr<Statement>> bindBody;
            auto bind = std::make_unique<Function>("bind", std::move(bindParams),
                                                   "IO<B>", std::move(bindBody));
            methods.push_back(std::move(bind));
        }
        else if (pattern == "Free")
        {
            // Free: AST for computations
            properties.push_back({"tree", "variant<Pure<A>, Impure<F, Free<F, A>>>"});

            // pure constructor
            std::vector<std::pair<std::string, std::string>> pureParams;
            pureParams.push_back({"a", "A"});
            std::vector<std::unique_ptr<Statement>> pureBody;
            auto pure = std::make_unique<Function>("pure", std::move(pureParams),
                                                   "Free<F, A>", std::move(pureBody));
            methods.push_back(std::move(pure));

            // impure constructor
            std::vector<std::pair<std::string, std::string>> impureParams;
            impureParams.push_back({"f", "F<Free<F, A>>"});
            std::vector<std::unique_ptr<Statement>> impureBody;
            auto impure = std::make_unique<Function>("impure", std::move(impureParams),
                                                     "Free<F, A>", std::move(impureBody));
            methods.push_back(std::move(impure));

            // interpret method
            std::vector<std::pair<std::string, std::string>> interpretParams;
            interpretParams.push_back({"interpreter", "function<M<A>(F<A>)>"});
            std::vector<std::unique_ptr<Statement>> interpretBody;
            auto interpret = std::make_unique<Function>("interpret", std::move(interpretParams),
                                                        "M<A>", std::move(interpretBody));
            methods.push_back(std::move(interpret));
        }
        else if (pattern == "Continuation")
        {
            // Continuation: CPS (Continuation-Passing Style)
            properties.push_back({"runCont", "function<R(function<R(A)>)>"});

            // callCC (call-with-current-continuation)
            std::vector<std::pair<std::string, std::string>> callCCParams;
            callCCParams.push_back({"f", "function<Cont<R, A>(function<Cont<R, B>(A)>)>"});
            std::vector<std::unique_ptr<Statement>> callCCBody;
            auto callCC = std::make_unique<Function>("callCC", std::move(callCCParams),
                                                     "Cont<R, A>", std::move(callCCBody));
            methods.push_back(std::move(callCC));

            // bind method
            std::vector<std::pair<std::string, std::string>> bindParams;
            bindParams.push_back({"f", "function<Cont<R, B>(A)>"});
            std::vector<std::unique_ptr<Statement>> bindBody;
            auto bind = std::make_unique<Function>("bind", std::move(bindParams),
                                                   "Cont<R, B>", std::move(bindBody));
            methods.push_back(std::move(bind));
        }
        else if (pattern == "Comonad")
        {
            // Comonad: dual of Monad
            properties.push_back({"value", "W<A>"});

            // extract (opposite of pure)
            std::vector<std::unique_ptr<Statement>> extractBody;
            auto extract = std::make_unique<Function>("extract",
                                                      std::vector<std::pair<std::string, std::string>>(),
                                                      "A", std::move(extractBody));
            methods.push_back(std::move(extract));

            // extend (opposite of bind)
            std::vector<std::pair<std::string, std::string>> extendParams;
            extendParams.push_back({"f", "function<B(W<A>)>"});
            std::vector<std::unique_ptr<Statement>> extendBody;
            auto extend = std::make_unique<Function>("extend", std::move(extendParams),
                                                     "W<B>", std::move(extendBody));
            methods.push_back(std::move(extend));

            // duplicate
            std::vector<std::unique_ptr<Statement>> duplicateBody;
            auto duplicate = std::make_unique<Function>("duplicate",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "W<W<A>>", std::move(duplicateBody));
            methods.push_back(std::move(duplicate));
        }
        else if (pattern == "Zipper")
        {
            // Zipper: cursor navigation in data structures
            properties.push_back({"left", "vector<T>"});
            properties.push_back({"focus", "T"});
            properties.push_back({"right", "vector<T>"});

            // moveLeft
            std::vector<std::unique_ptr<Statement>> moveLeftBody;
            auto moveLeft = std::make_unique<Function>("moveLeft",
                                                       std::vector<std::pair<std::string, std::string>>(),
                                                       "Zipper<T>", std::move(moveLeftBody));
            methods.push_back(std::move(moveLeft));

            // moveRight
            std::vector<std::unique_ptr<Statement>> moveRightBody;
            auto moveRight = std::make_unique<Function>("moveRight",
                                                        std::vector<std::pair<std::string, std::string>>(),
                                                        "Zipper<T>", std::move(moveRightBody));
            methods.push_back(std::move(moveRight));

            // update
            std::vector<std::pair<std::string, std::string>> updateParams;
            updateParams.push_back({"f", "function<T(T)>"});
            std::vector<std::unique_ptr<Statement>> updateBody;
            auto update = std::make_unique<Function>("update", std::move(updateParams),
                                                     "Zipper<T>", std::move(updateBody));
            methods.push_back(std::move(update));
        }
        else if (pattern == "Church")
        {
            // Church: data encoded as functions
            properties.push_back({"churchEncoding", "function<R(function<R(A)>, function<R()>)>"});

            // fold (church catamorphism)
            std::vector<std::pair<std::string, std::string>> foldParams;
            foldParams.push_back({"onValue", "function<R(A)>"});
            foldParams.push_back({"onEmpty", "function<R()>"});
            std::vector<std::unique_ptr<Statement>> foldBody;
            auto fold = std::make_unique<Function>("fold", std::move(foldParams),
                                                   "R", std::move(foldBody));
            methods.push_back(std::move(fold));

            // encode
            std::vector<std::pair<std::string, std::string>> encodeParams;
            encodeParams.push_back({"value", "A"});
            std::vector<std::unique_ptr<Statement>> encodeBody;
            auto encode = std::make_unique<Function>("encode", std::move(encodeParams),
                                                     "Church<A>", std::move(encodeBody));
            methods.push_back(std::move(encode));
        }
        else if (pattern == "AlgebraicEffect")
        {
            // AlgebraicEffect: composable effects with handlers
            properties.push_back({"effect", "Effect"});
            properties.push_back({"handler", "function<A(Effect)>"});

            // perform (trigger effect)
            std::vector<std::pair<std::string, std::string>> performParams;
            performParams.push_back({"eff", "Effect"});
            std::vector<std::unique_ptr<Statement>> performBody;
            auto perform = std::make_unique<Function>("perform", std::move(performParams),
                                                      "A", std::move(performBody));
            methods.push_back(std::move(perform));

            // handle (install handler)
            std::vector<std::pair<std::string, std::string>> handleParams;
            handleParams.push_back({"h", "function<B(Effect)>"});
            std::vector<std::unique_ptr<Statement>> handleBody;
            auto handle = std::make_unique<Function>("handle", std::move(handleParams),
                                                     "B", std::move(handleBody));
            methods.push_back(std::move(handle));
        }
        else if (pattern == "Morphism")
        {
            // Morphism: catamorphism (fold) and anamorphism (unfold)
            properties.push_back({"algebra", "function<A(F<A>)>"});
            properties.push_back({"coalgebra", "function<F<A>(A)>"});

            // cata (catamorphism - fold/consume structure)
            std::vector<std::pair<std::string, std::string>> cataParams;
            cataParams.push_back({"alg", "function<A(F<A>)>"});
            cataParams.push_back({"structure", "Fix<F>"});
            std::vector<std::unique_ptr<Statement>> cataBody;
            auto cata = std::make_unique<Function>("cata", std::move(cataParams),
                                                   "A", std::move(cataBody));
            methods.push_back(std::move(cata));

            // ana (anamorphism - unfold/produce structure)
            std::vector<std::pair<std::string, std::string>> anaParams;
            anaParams.push_back({"coalg", "function<F<A>(A)>"});
            anaParams.push_back({"seed", "A"});
            std::vector<std::unique_ptr<Statement>> anaBody;
            auto ana = std::make_unique<Function>("ana", std::move(anaParams),
                                                  "Fix<F>", std::move(anaBody));
            methods.push_back(std::move(ana));

            // hylo (hylomorphism - unfold then fold)
            std::vector<std::pair<std::string, std::string>> hyloParams;
            hyloParams.push_back({"alg", "function<B(F<B>)>"});
            hyloParams.push_back({"coalg", "function<F<A>(A)>"});
            hyloParams.push_back({"seed", "A"});
            std::vector<std::unique_ptr<Statement>> hyloBody;
            auto hylo = std::make_unique<Function>("hylo", std::move(hyloParams),
                                                   "B", std::move(hyloBody));
            methods.push_back(std::move(hylo));
        }

        // Create and return the class declaration
        auto classDecl = std::make_unique<ClassDecl>(className, "",
                                                     std::move(properties), std::move(methods), std::move(constructor));
        classDecl->designPattern = pattern;

        return classDecl;
    }

} // namespace lpp
