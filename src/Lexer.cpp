#include "Lexer.h"
#include <cctype>
#include <map>

namespace lpp
{

    static const std::map<std::string, TokenType> keywords = {
        {"let", TokenType::LET},
        {"const", TokenType::CONST},
        {"var", TokenType::VAR},
        {"mut", TokenType::MUT},
        {"fn", TokenType::FN},
        {"type", TokenType::TYPE},
        {"if", TokenType::IF},
        {"else", TokenType::ELSE},
        {"while", TokenType::WHILE},
        {"for", TokenType::FOR},
        {"do", TokenType::DO},
        {"switch", TokenType::SWITCH},
        {"case", TokenType::CASE},
        {"default", TokenType::DEFAULT},
        {"break", TokenType::BREAK},
        {"continue", TokenType::CONTINUE},
        {"return", TokenType::RETURN},
        {"true", TokenType::TRUE},
        {"false", TokenType::FALSE},
        {"and", TokenType::AND},
        {"or", TokenType::OR},
        {"not", TokenType::NOT},
        {"in", TokenType::IN},
        {"of", TokenType::OF},
        {"match", TokenType::MATCH},
        {"class", TokenType::CLASS},
        {"enum", TokenType::ENUM},
        {"constructor", TokenType::CONSTRUCTOR},
        {"extends", TokenType::EXTENDS},
        {"implements", TokenType::IMPLEMENTS},
        {"abstract", TokenType::ABSTRACT},
        {"interface", TokenType::INTERFACE},
        {"static", TokenType::STATIC},
        {"override", TokenType::OVERRIDE},
        {"public", TokenType::PUBLIC},
        {"private", TokenType::PRIVATE},
        {"protected", TokenType::PROTECTED},
        {"this", TokenType::THIS},
        {"super", TokenType::SUPER},
        {"new", TokenType::NEW},
        {"async", TokenType::ASYNC},
        {"await", TokenType::AWAIT},
        {"try", TokenType::TRY},
        {"catch", TokenType::CATCH},
        {"finally", TokenType::FINALLY},
        {"throw", TokenType::THROW},
        {"import", TokenType::IMPORT},
        {"export", TokenType::EXPORT},
        {"from", TokenType::FROM},
        {"as", TokenType::AS},
        {"autopattern", TokenType::AUTOPATTERN},
        {"quantum", TokenType::QUANTUM},
        {"experimental", TokenType::EXPERIMENTAL},
        {"observe", TokenType::OBSERVE},
        {"entangle", TokenType::ENTANGLE},
        {"superposition", TokenType::SUPERPOSITION},
        {"null", TokenType::NULL_KW},
        {"undefined", TokenType::UNDEFINED},
        {"yield", TokenType::YIELD},
        {"typeof", TokenType::TYPEOF},
        {"instanceof", TokenType::INSTANCEOF},
        {"get", TokenType::GET},
        {"set", TokenType::SET},
        {"when", TokenType::WHEN},
        {"test", TokenType::TEST},
        {"assert", TokenType::ASSERT},
        {"macro", TokenType::MACRO},
        {"extern", TokenType::EXTERN},
        {"int", TokenType::TYPE_INT},
        {"float", TokenType::TYPE_FLOAT},
        {"string", TokenType::TYPE_STRING},
        {"bool", TokenType::TYPE_BOOL},
        {"void", TokenType::TYPE_VOID},
        {"any", TokenType::TYPE_ANY},
        {"unknown", TokenType::TYPE_UNKNOWN},
        {"pragma", TokenType::PRAGMA},
        {"paradigm", TokenType::IDENTIFIER}, // parsed as identifier, context-sensitive
        {"hybrid", TokenType::IDENTIFIER},
        {"functional", TokenType::IDENTIFIER},
        {"imperative", TokenType::IDENTIFIER},
        {"oop", TokenType::IDENTIFIER},
        {"golfed", TokenType::IDENTIFIER}};

    Lexer::Lexer(const std::string &source) : source(source) {}

    std::vector<Token> Lexer::tokenize()
    {
        std::vector<Token> tokens;

        while (!isAtEnd())
        {
            skipWhitespace();
            if (isAtEnd())
                break;

            char c = peek();

            // Handle preprocessor directives (pragmas)
            if (c == '#')
            {
                tokens.push_back(pragma());
                continue;
            }

            // Skip comments
            if (c == '/' && current + 1 < source.length())
            {
                if (source[current + 1] == '/')
                {
                    skipComment();
                    continue;
                }
                else if (source[current + 1] == '*')
                {
                    skipBlockComment();
                    continue;
                }
            }

            // Numbers
            if (std::isdigit(c))
            {
                tokens.push_back(number());
                continue;
            }

            // Strings
            if (c == '"')
            {
                tokens.push_back(string());
                continue;
            }

            // Identifiers and keywords
            if (std::isalpha(c) || c == '_')
            {
                tokens.push_back(identifier());
                continue;
            }

            // Single and multi-character tokens
            advance();
            switch (c)
            {
            case '(':
                tokens.push_back(makeToken(TokenType::LPAREN, "("));
                break;
            case ')':
                tokens.push_back(makeToken(TokenType::RPAREN, ")"));
                break;
            case '{':
                tokens.push_back(makeToken(TokenType::LBRACE, "{"));
                break;
            case '}':
                tokens.push_back(makeToken(TokenType::RBRACE, "}"));
                break;
            case '[':
                tokens.push_back(makeToken(TokenType::LBRACKET, "["));
                break;
            case ']':
                tokens.push_back(makeToken(TokenType::RBRACKET, "]"));
                break;
            case ';':
                tokens.push_back(makeToken(TokenType::SEMICOLON, ";"));
                break;
            case ',':
                tokens.push_back(makeToken(TokenType::COMMA, ","));
                break;
            case ':':
                tokens.push_back(makeToken(TokenType::COLON, ":"));
                break;
            case '$':
                tokens.push_back(makeToken(TokenType::DOLLAR, "$"));
                break;
            case '`':
                tokens.push_back(makeToken(TokenType::BACKTICK, "`"));
                break;
            case '@':
                tokens.push_back(makeToken(TokenType::AT, "@"));
                break;
            case '~':
                if (match('>'))
                {
                    tokens.push_back(makeToken(TokenType::TILDE_GT, "~>"));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::TILDE, "~"));
                }
                break;
            case '\\':
                tokens.push_back(makeToken(TokenType::BACKSLASH, "\\"));
                break;
            case '^':
                tokens.push_back(makeToken(TokenType::CARET, "^"));
                break;
            case '+':
                if (match('+'))
                {
                    tokens.push_back(makeToken(TokenType::PLUS_PLUS, "++"));
                }
                else if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::PLUS_EQUAL, "+="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::PLUS, "+"));
                }
                break;
            case '-':
                if (match('>'))
                {
                    tokens.push_back(makeToken(TokenType::ARROW, "->"));
                }
                else if (match('-'))
                {
                    tokens.push_back(makeToken(TokenType::MINUS_MINUS, "--"));
                }
                else if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::MINUS_EQUAL, "-="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::MINUS, "-"));
                }
                break;
            case '*':
                if (match('*'))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::POWER_EQUAL, "**="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::POWER, "**"));
                    }
                }
                else if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::STAR_EQUAL, "*="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::STAR, "*"));
                }
                break;
            case '/':
                if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::SLASH_EQUAL, "/="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::SLASH, "/"));
                }
                break;
            case '%':
                if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::PERCENT_EQUAL, "%="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::PERCENT, "%"));
                }
                break;
            case '=':
                if (match('='))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::EQUAL_EQUAL_EQUAL, "==="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::EQUAL_EQUAL, "=="));
                    }
                }
                else if (match('>'))
                {
                    tokens.push_back(makeToken(TokenType::FAT_ARROW, "=>"));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::EQUAL, "="));
                }
                break;
            case '!':
                if (match('!'))
                {
                    // !! operator variants
                    if (match('<'))
                    {
                        tokens.push_back(makeToken(TokenType::BANG_BANG_LESS, "!!<"));
                    }
                    else if (match('>'))
                    {
                        tokens.push_back(makeToken(TokenType::BANG_BANG_GREATER, "!!>"));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::BANG_BANG, "!!"));
                    }
                }
                else if (match('='))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::BANG_EQUAL_EQUAL, "!=="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::BANG_EQUAL, "!="));
                    }
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::NOT, "!"));
                }
                break;
            case '<':
                if (match('-'))
                {
                    tokens.push_back(makeToken(TokenType::ARROW_LEFT, "<-"));
                }
                else if (match('<'))
                {
                    tokens.push_back(makeToken(TokenType::LESS_LESS, "<<"));
                }
                else if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::LESS_EQUAL, "<="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::LESS, "<"));
                }
                break;
            case '>':
                if (match('>'))
                {
                    if (match('>'))
                    {
                        tokens.push_back(makeToken(TokenType::GREATER_GREATER_GREATER, ">>>"));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::GREATER_GREATER, ">>"));
                    }
                }
                else if (match('='))
                {
                    tokens.push_back(makeToken(TokenType::GREATER_EQUAL, ">="));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::GREATER, ">"));
                }
                break;
            case '&':
                if (match('&'))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::AMP_AMP_EQUAL, "&&="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::AMP_AMP, "&&"));
                    }
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::AMP, "&"));
                }
                break;
            case '|':
                if (match('|'))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::PIPE_PIPE_EQUAL, "||="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::PIPE_PIPE, "||"));
                    }
                }
                else if (match('>'))
                {
                    tokens.push_back(makeToken(TokenType::PIPE_GT, "|>"));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::PIPE, "|"));
                }
                break;
            case '?':
                if (match('?'))
                {
                    if (match('='))
                    {
                        tokens.push_back(makeToken(TokenType::QUESTION_QUESTION_EQUAL, "??="));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::QUESTION_QUESTION, "??"));
                    }
                }
                else if (match('.'))
                {
                    tokens.push_back(makeToken(TokenType::QUESTION_DOT, "?."));
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::QUESTION, "?"));
                }
                break;
            case '.':
                if (match('.'))
                {
                    if (match('.'))
                    {
                        tokens.push_back(makeToken(TokenType::DOT_DOT_DOT, "..."));
                    }
                    else
                    {
                        tokens.push_back(makeToken(TokenType::DOT_DOT, ".."));
                    }
                }
                else
                {
                    tokens.push_back(makeToken(TokenType::DOT, "."));
                }
                break;
            default:
                tokens.push_back(makeToken(TokenType::INVALID, std::string(1, c)));
                break;
            }
        }

        tokens.push_back(Token(TokenType::END_OF_FILE, "", line, column));
        return tokens;
    }

    char Lexer::peek() const
    {
        if (isAtEnd())
            return '\0';
        return source[current];
    }

    char Lexer::advance()
    {
        char c = source[current++];
        if (c == '\n')
        {
            line++;
            column = 1;
        }
        else
        {
            column++;
        }
        return c;
    }

    bool Lexer::isAtEnd() const
    {
        return current >= source.length();
    }

    bool Lexer::match(char expected)
    {
        if (isAtEnd() || source[current] != expected)
            return false;
        advance();
        return true;
    }

    void Lexer::skipWhitespace()
    {
        while (!isAtEnd() && std::isspace(peek()))
        {
            advance();
        }
    }

    void Lexer::skipComment()
    {
        // Skip //
        advance();
        advance();

        while (!isAtEnd() && peek() != '\n')
        {
            advance();
        }
    }

    void Lexer::skipBlockComment()
    {
        // Skip /*
        advance();
        advance();

        while (!isAtEnd())
        {
            if (peek() == '*' && current + 1 < source.length() && source[current + 1] == '/')
            {
                advance(); // skip *
                advance(); // skip /
                return;
            }
            advance();
        }
    }

    Token Lexer::makeToken(TokenType type, const std::string &lexeme)
    {
        return Token(type, lexeme, line, column - lexeme.length());
    }

    Token Lexer::number()
    {
        size_t start = current;
        int startCol = column;

        while (!isAtEnd() && std::isdigit(peek()))
        {
            advance();
        }

        // Handle decimal point
        if (!isAtEnd() && peek() == '.' && current + 1 < source.length() && std::isdigit(source[current + 1]))
        {
            advance(); // consume '.'
            while (!isAtEnd() && std::isdigit(peek()))
            {
                advance();
            }
        }

        std::string lexeme = source.substr(start, current - start);
        return Token(TokenType::NUMBER, lexeme, line, startCol);
    }

    Token Lexer::string()
    {
        int startCol = column;
        advance(); // consume opening "

        std::string result;
        while (!isAtEnd() && peek() != '"')
        {
            if (peek() == '\\')
            {
                advance(); // consume backslash
                if (isAtEnd())
                {
                    return Token(TokenType::INVALID, "Unterminated string with escape", line, startCol);
                }

                char escaped = peek();
                switch (escaped)
                {
                case 'n':
                    result += '\n';
                    break;
                case 't':
                    result += '\t';
                    break;
                case 'r':
                    result += '\r';
                    break;
                case '\\':
                    result += '\\';
                    break;
                case '"':
                    result += '"';
                    break;
                case '0':
                    result += "\\0";
                    break;
                default:
                    result += '\\';
                    result += escaped;
                    break;
                }
                advance();
            }
            else
            {
                result += peek();
                advance();
            }
        }

        if (isAtEnd())
        {
            return Token(TokenType::INVALID, "Unterminated string", line, startCol);
        }

        advance(); // consume closing "

        return Token(TokenType::STRING, result, line, startCol);
    }

    Token Lexer::identifier()
    {
        size_t start = current;
        int startCol = column;

        while (!isAtEnd() && (std::isalnum(peek()) || peek() == '_'))
        {
            advance();
        }

        std::string lexeme = source.substr(start, current - start);
        TokenType type = identifierType(lexeme);

        return Token(type, lexeme, line, startCol);
    }

    TokenType Lexer::identifierType(const std::string &text)
    {
        auto it = keywords.find(text);
        return (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    }

    Token Lexer::pragma()
    {
        int startCol = column;
        advance(); // consume '#'

        // Skip whitespace after #
        while (!isAtEnd() && (peek() == ' ' || peek() == '\t'))
        {
            advance();
        }

        // Read the pragma directive (entire line)
        size_t start = current;
        while (!isAtEnd() && peek() != '\n')
        {
            advance();
        }

        std::string lexeme = source.substr(start, current - start);
        return Token(TokenType::PRAGMA, lexeme, line, startCol);
    }

} // namespace lpp
