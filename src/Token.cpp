#include "Token.h"
#include <map>

namespace lpp
{

    static const std::map<TokenType, std::string> tokenNames = {
        // Literals
        {TokenType::NUMBER, "NUMBER"},
        {TokenType::STRING, "STRING"},
        {TokenType::TEMPLATE_STRING, "TEMPLATE_STRING"},
        {TokenType::IDENTIFIER, "IDENTIFIER"},

        // Keywords
        {TokenType::LET, "LET"},
        {TokenType::CONST, "CONST"},
        {TokenType::VAR, "VAR"},
        {TokenType::FN, "FN"},
        {TokenType::TYPE, "TYPE"},
        {TokenType::IF, "IF"},
        {TokenType::ELSE, "ELSE"},
        {TokenType::WHILE, "WHILE"},
        {TokenType::FOR, "FOR"},
        {TokenType::DO, "DO"},
        {TokenType::SWITCH, "SWITCH"},
        {TokenType::CASE, "CASE"},
        {TokenType::DEFAULT, "DEFAULT"},
        {TokenType::BREAK, "BREAK"},
        {TokenType::CONTINUE, "CONTINUE"},
        {TokenType::RETURN, "RETURN"},
        {TokenType::TRUE, "TRUE"},
        {TokenType::FALSE, "FALSE"},
        {TokenType::AND, "AND"},
        {TokenType::OR, "OR"},
        {TokenType::NOT, "NOT"},
        {TokenType::IN, "IN"},
        {TokenType::OF, "OF"},
        {TokenType::MATCH, "MATCH"},
        {TokenType::CLASS, "CLASS"},
        {TokenType::CONSTRUCTOR, "CONSTRUCTOR"},
        {TokenType::EXTENDS, "EXTENDS"},
        {TokenType::IMPLEMENTS, "IMPLEMENTS"},
        {TokenType::ABSTRACT, "ABSTRACT"},
        {TokenType::INTERFACE, "INTERFACE"},
        {TokenType::STATIC, "STATIC"},
        {TokenType::OVERRIDE, "OVERRIDE"},
        {TokenType::PUBLIC, "PUBLIC"},
        {TokenType::PRIVATE, "PRIVATE"},
        {TokenType::PROTECTED, "PROTECTED"},
        {TokenType::THIS, "THIS"},
        {TokenType::SUPER, "SUPER"},
        {TokenType::NEW, "NEW"},
        {TokenType::ASYNC, "ASYNC"},
        {TokenType::AWAIT, "AWAIT"},
        {TokenType::TRY, "TRY"},
        {TokenType::CATCH, "CATCH"},
        {TokenType::FINALLY, "FINALLY"},
        {TokenType::THROW, "THROW"},
        {TokenType::IMPORT, "IMPORT"},
        {TokenType::EXPORT, "EXPORT"},
        {TokenType::FROM, "FROM"},
        {TokenType::AS, "AS"},
        {TokenType::NULL_KW, "NULL"},
        {TokenType::UNDEFINED, "UNDEFINED"},

        // Types
        {TokenType::TYPE_INT, "TYPE_INT"},
        {TokenType::TYPE_FLOAT, "TYPE_FLOAT"},
        {TokenType::TYPE_STRING, "TYPE_STRING"},
        {TokenType::TYPE_BOOL, "TYPE_BOOL"},
        {TokenType::TYPE_VOID, "TYPE_VOID"},
        {TokenType::TYPE_ANY, "TYPE_ANY"},
        {TokenType::TYPE_UNKNOWN, "TYPE_UNKNOWN"},

        // Operators
        {TokenType::PLUS, "PLUS"},
        {TokenType::MINUS, "MINUS"},
        {TokenType::STAR, "STAR"},
        {TokenType::SLASH, "SLASH"},
        {TokenType::PERCENT, "PERCENT"},
        {TokenType::POWER, "POWER"},
        {TokenType::EQUAL, "EQUAL"},
        {TokenType::EQUAL_EQUAL, "EQUAL_EQUAL"},
        {TokenType::EQUAL_EQUAL_EQUAL, "EQUAL_EQUAL_EQUAL"},
        {TokenType::BANG_EQUAL, "BANG_EQUAL"},
        {TokenType::BANG_EQUAL_EQUAL, "BANG_EQUAL_EQUAL"},
        {TokenType::LESS, "LESS"},
        {TokenType::LESS_EQUAL, "LESS_EQUAL"},
        {TokenType::GREATER, "GREATER"},
        {TokenType::GREATER_EQUAL, "GREATER_EQUAL"},
        {TokenType::PLUS_PLUS, "PLUS_PLUS"},
        {TokenType::MINUS_MINUS, "MINUS_MINUS"},
        {TokenType::PLUS_EQUAL, "PLUS_EQUAL"},
        {TokenType::MINUS_EQUAL, "MINUS_EQUAL"},
        {TokenType::STAR_EQUAL, "STAR_EQUAL"},
        {TokenType::SLASH_EQUAL, "SLASH_EQUAL"},
        {TokenType::PERCENT_EQUAL, "PERCENT_EQUAL"},
        {TokenType::POWER_EQUAL, "POWER_EQUAL"},
        {TokenType::AMP_AMP, "AMP_AMP"},
        {TokenType::PIPE_PIPE, "PIPE_PIPE"},
        {TokenType::AMP_AMP_EQUAL, "AMP_AMP_EQUAL"},
        {TokenType::PIPE_PIPE_EQUAL, "PIPE_PIPE_EQUAL"},
        {TokenType::QUESTION_QUESTION, "QUESTION_QUESTION"},
        {TokenType::QUESTION_QUESTION_EQUAL, "QUESTION_QUESTION_EQUAL"},

        // Bitwise
        {TokenType::AMP, "AMP"},
        {TokenType::PIPE, "PIPE"},
        {TokenType::CARET, "CARET"},
        {TokenType::TILDE, "TILDE"},
        {TokenType::BACKSLASH, "BACKSLASH"},
        {TokenType::LESS_LESS, "LESS_LESS"},
        {TokenType::GREATER_GREATER, "GREATER_GREATER"},
        {TokenType::GREATER_GREATER_GREATER, "GREATER_GREATER_GREATER"},

        // Delimiters
        {TokenType::LPAREN, "LPAREN"},
        {TokenType::RPAREN, "RPAREN"},
        {TokenType::LBRACE, "LBRACE"},
        {TokenType::RBRACE, "RBRACE"},
        {TokenType::LBRACKET, "LBRACKET"},
        {TokenType::RBRACKET, "RBRACKET"},
        {TokenType::SEMICOLON, "SEMICOLON"},
        {TokenType::COMMA, "COMMA"},
        {TokenType::COLON, "COLON"},
        {TokenType::ARROW, "ARROW"},
        {TokenType::DOT, "DOT"},
        {TokenType::DOT_DOT, "DOT_DOT"},
        {TokenType::DOT_DOT_DOT, "DOT_DOT_DOT"},
        {TokenType::QUESTION, "QUESTION"},
        {TokenType::QUESTION_DOT, "QUESTION_DOT"},
        {TokenType::DOLLAR, "DOLLAR"},
        {TokenType::PIPE_GT, "PIPE_GT"},
        {TokenType::BACKTICK, "BACKTICK"},
        {TokenType::AT, "AT"},

        // Special
        {TokenType::END_OF_FILE, "EOF"},
        {TokenType::INVALID, "INVALID"}};

    std::string tokenTypeToString(TokenType type)
    {
        auto it = tokenNames.find(type);
        return (it != tokenNames.end()) ? it->second : "UNKNOWN";
    }

} // namespace lpp
