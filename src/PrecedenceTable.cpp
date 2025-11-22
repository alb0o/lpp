#include "PrecedenceTable.h"
#include <stdexcept>

namespace lpp
{
    // ========== PrecedenceTable Implementation ==========

    PrecedenceTable::PrecedenceTable()
    {
        initializeCoreOperators();
    }

    void PrecedenceTable::initializeCoreOperators()
    {
        // Core precedence levels (0-100, higher = tighter binding)

        // Level 90: Function composition (right-assoc)
        tokenFixity[TokenType::DOT] = FixityInfo(90, Associativity::RIGHT, true);

        // Level 80: Exponentiation (right-assoc)
        tokenFixity[TokenType::POWER] = FixityInfo(80, Associativity::RIGHT, true);
        tokenFixity[TokenType::CARET] = FixityInfo(80, Associativity::RIGHT, true);

        // Level 70: Multiplicative (left-assoc)
        tokenFixity[TokenType::STAR] = FixityInfo(70, Associativity::LEFT, true);
        tokenFixity[TokenType::SLASH] = FixityInfo(70, Associativity::LEFT, true);
        tokenFixity[TokenType::PERCENT] = FixityInfo(70, Associativity::LEFT, true);

        // Level 60: Additive (left-assoc)
        tokenFixity[TokenType::PLUS] = FixityInfo(60, Associativity::LEFT, true);
        tokenFixity[TokenType::MINUS] = FixityInfo(60, Associativity::LEFT, true);

        // Level 50: Range (left-assoc)
        tokenFixity[TokenType::DOT_DOT] = FixityInfo(50, Associativity::LEFT, true);

        // Level 40: Comparison (left-assoc)
        tokenFixity[TokenType::LESS] = FixityInfo(40, Associativity::LEFT, true);
        tokenFixity[TokenType::LESS_EQUAL] = FixityInfo(40, Associativity::LEFT, true);
        tokenFixity[TokenType::GREATER] = FixityInfo(40, Associativity::LEFT, true);
        tokenFixity[TokenType::GREATER_EQUAL] = FixityInfo(40, Associativity::LEFT, true);

        // Level 35: Equality (left-assoc)
        tokenFixity[TokenType::EQUAL_EQUAL] = FixityInfo(35, Associativity::LEFT, true);
        tokenFixity[TokenType::BANG_EQUAL] = FixityInfo(35, Associativity::LEFT, true);
        tokenFixity[TokenType::EQUAL_EQUAL_EQUAL] = FixityInfo(35, Associativity::LEFT, true);
        tokenFixity[TokenType::BANG_EQUAL_EQUAL] = FixityInfo(35, Associativity::LEFT, true);

        // Level 30: Membership (left-assoc)
        tokenFixity[TokenType::IN] = FixityInfo(30, Associativity::LEFT, true);

        // Level 25: Logical AND (left-assoc)
        tokenFixity[TokenType::AND] = FixityInfo(25, Associativity::LEFT, true);
        tokenFixity[TokenType::AMP_AMP] = FixityInfo(25, Associativity::LEFT, true);

        // Level 20: Logical OR (left-assoc)
        tokenFixity[TokenType::OR] = FixityInfo(20, Associativity::LEFT, true);
        tokenFixity[TokenType::PIPE_PIPE] = FixityInfo(20, Associativity::LEFT, true); // Level 15: Nullish coalescing (right-assoc)
        tokenFixity[TokenType::QUESTION_QUESTION] = FixityInfo(15, Associativity::RIGHT, true);

        // Level 10: Pipeline (left-assoc)
        tokenFixity[TokenType::PIPE_GT] = FixityInfo(10, Associativity::LEFT, true);

        // Level 5: Assignment (right-assoc)
        tokenFixity[TokenType::EQUAL] = FixityInfo(5, Associativity::RIGHT, true);
        tokenFixity[TokenType::PLUS_EQUAL] = FixityInfo(5, Associativity::RIGHT, true);
        tokenFixity[TokenType::MINUS_EQUAL] = FixityInfo(5, Associativity::RIGHT, true);
        tokenFixity[TokenType::STAR_EQUAL] = FixityInfo(5, Associativity::RIGHT, true);
        tokenFixity[TokenType::SLASH_EQUAL] = FixityInfo(5, Associativity::RIGHT, true);
    }

    FixityInfo PrecedenceTable::getFixity(TokenType op) const
    {
        auto it = tokenFixity.find(op);
        if (it != tokenFixity.end())
        {
            return it->second;
        }
        return FixityInfo(0, Associativity::LEFT, false); // Default: lowest precedence
    }

    FixityInfo PrecedenceTable::getFixity(const std::string &opName) const
    {
        auto it = customFixity.find(opName);
        if (it != customFixity.end())
        {
            return it->second;
        }

        // Try to convert to TokenType
        TokenType type = operatorNameToType(opName);
        if (type != TokenType::END_OF_FILE)
        {
            return getFixity(type);
        }

        return FixityInfo(0, Associativity::LEFT, false);
    }

    void PrecedenceTable::setFixity(TokenType op, int precedence, Associativity assoc)
    {
        // Check if it's a core operator
        auto it = tokenFixity.find(op);
        if (it != tokenFixity.end() && it->second.isCore)
        {
            // Allow override in local scope, but mark as warning-worthy
            tokenFixity[op] = FixityInfo(precedence, assoc, false); // Remove core flag
        }
        else
        {
            tokenFixity[op] = FixityInfo(precedence, assoc, false);
        }
    }

    void PrecedenceTable::setFixity(const std::string &opName, int precedence, Associativity assoc)
    {
        customFixity[opName] = FixityInfo(precedence, assoc, false);
    }

    bool PrecedenceTable::hasOperator(TokenType op) const
    {
        return tokenFixity.find(op) != tokenFixity.end();
    }

    bool PrecedenceTable::hasOperator(const std::string &opName) const
    {
        if (customFixity.find(opName) != customFixity.end())
        {
            return true;
        }
        return operatorNameToType(opName) != TokenType::END_OF_FILE;
    }

    std::unique_ptr<PrecedenceTable> PrecedenceTable::clone() const
    {
        auto newTable = std::make_unique<PrecedenceTable>();
        newTable->tokenFixity = this->tokenFixity;
        newTable->customFixity = this->customFixity;
        return newTable;
    }

    void PrecedenceTable::resetToCore()
    {
        tokenFixity.clear();
        customFixity.clear();
        initializeCoreOperators();
    }

    std::vector<std::string> PrecedenceTable::getCustomOperators() const
    {
        std::vector<std::string> result;
        for (const auto &[name, _] : customFixity)
        {
            result.push_back(name);
        }
        return result;
    }

    TokenType PrecedenceTable::operatorNameToType(const std::string &name) const
    {
        // Map common operator names to TokenType
        static const std::unordered_map<std::string, TokenType> nameToType = {
            {"+", TokenType::PLUS},
            {"-", TokenType::MINUS},
            {"*", TokenType::STAR},
            {"/", TokenType::SLASH},
            {"%", TokenType::PERCENT},
            {"**", TokenType::POWER},
            {"^", TokenType::CARET},
            {"==", TokenType::EQUAL_EQUAL},
            {"!=", TokenType::BANG_EQUAL},
            {"<", TokenType::LESS},
            {">", TokenType::GREATER},
            {"<=", TokenType::LESS_EQUAL},
            {">=", TokenType::GREATER_EQUAL},
            {"&&", TokenType::AMP_AMP},
            {"||", TokenType::PIPE_PIPE},
            {"and", TokenType::AND},
            {"or", TokenType::OR},
            {"..", TokenType::DOT_DOT},
            {"|>", TokenType::PIPE_GT},
            {"??", TokenType::QUESTION_QUESTION},
        };

        auto it = nameToType.find(name);
        if (it != nameToType.end())
        {
            return it->second;
        }
        return TokenType::END_OF_FILE; // Not found
    }

    // ========== NotationContext Implementation ==========

    NotationContext::NotationContext()
    {
        // Start with default math mode
        pushMath();
    }

    void NotationContext::pushMath()
    {
        auto table = createTableForMode(NotationMode::MATH);
        scopes.emplace_back(NotationMode::MATH, std::move(table), "math");
    }

    void NotationContext::pushLinear()
    {
        auto table = createTableForMode(NotationMode::LINEAR);
        scopes.emplace_back(NotationMode::LINEAR, std::move(table), "linear");
    }

    void NotationContext::pushCustom(const std::string &name)
    {
        // Start with math table, user can modify it
        auto table = createTableForMode(NotationMode::CUSTOM);
        scopes.emplace_back(NotationMode::CUSTOM, std::move(table), name);
    }

    void NotationContext::pushCustom(std::unique_ptr<PrecedenceTable> table)
    {
        scopes.emplace_back(NotationMode::CUSTOM, std::move(table), "custom");
    }

    void NotationContext::pop()
    {
        if (scopes.size() > 1) // Keep at least one scope
        {
            scopes.pop_back();
        }
    }

    const PrecedenceTable &NotationContext::current() const
    {
        if (scopes.empty())
        {
            throw std::runtime_error("NotationContext: no active scope");
        }
        return *scopes.back().table;
    }

    PrecedenceTable &NotationContext::currentMutable()
    {
        if (scopes.empty())
        {
            throw std::runtime_error("NotationContext: no active scope");
        }
        return *scopes.back().table;
    }

    bool NotationContext::isLinearMode() const
    {
        return !scopes.empty() && scopes.back().mode == NotationMode::LINEAR;
    }

    NotationMode NotationContext::currentMode() const
    {
        if (scopes.empty())
        {
            return NotationMode::MATH;
        }
        return scopes.back().mode;
    }

    std::unique_ptr<PrecedenceTable> NotationContext::createTableForMode(NotationMode mode)
    {
        auto table = std::make_unique<PrecedenceTable>();

        if (mode == NotationMode::LINEAR)
        {
            // In linear mode, all operators have same precedence (50) and are left-associative
            // We override the core operators to have uniform precedence
            const int LINEAR_PRECEDENCE = 50;

            table->setFixity(TokenType::PLUS, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::MINUS, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::STAR, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::SLASH, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::PERCENT, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::POWER, LINEAR_PRECEDENCE, Associativity::LEFT); // Even power!
            table->setFixity(TokenType::LESS, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::GREATER, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::LESS_EQUAL, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::GREATER_EQUAL, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::EQUAL_EQUAL, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::BANG_EQUAL, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::AMP_AMP, LINEAR_PRECEDENCE, Associativity::LEFT);
            table->setFixity(TokenType::PIPE_PIPE, LINEAR_PRECEDENCE, Associativity::LEFT);
        }
        // MATH and CUSTOM modes use default core precedence

        return table;
    }

} // namespace lpp
