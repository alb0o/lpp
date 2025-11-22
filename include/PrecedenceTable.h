#ifndef PRECEDENCE_TABLE_H
#define PRECEDENCE_TABLE_H

#include "Token.h"
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

namespace lpp
{
    // Operator associativity
    enum class Associativity
    {
        LEFT,
        RIGHT,
        NONE // Non-associative (error if used without parens)
    };

    // Operator fixity information
    struct FixityInfo
    {
        int precedence; // Higher = tighter binding (0-100)
        Associativity assoc;
        bool isCore; // Core operators cannot be redefined globally

        FixityInfo(int prec = 0, Associativity a = Associativity::LEFT, bool core = false)
            : precedence(prec), assoc(a), isCore(core) {}
    };

    // Precedence table for operators
    class PrecedenceTable
    {
    public:
        PrecedenceTable();

        // Get fixity for an operator
        FixityInfo getFixity(TokenType op) const;
        FixityInfo getFixity(const std::string &opName) const;

        // Set custom fixity (for notation blocks)
        void setFixity(TokenType op, int precedence, Associativity assoc);
        void setFixity(const std::string &opName, int precedence, Associativity assoc);

        // Check if operator exists
        bool hasOperator(TokenType op) const;
        bool hasOperator(const std::string &opName) const;

        // Clone table (for notation scopes)
        std::unique_ptr<PrecedenceTable> clone() const;

        // Reset to default core precedence
        void resetToCore();

        // Get all custom operators (non-core)
        std::vector<std::string> getCustomOperators() const;

    private:
        // Map from TokenType to fixity
        std::unordered_map<TokenType, FixityInfo> tokenFixity;

        // Map from operator name to fixity (for custom operators)
        std::unordered_map<std::string, FixityInfo> customFixity;

        // Initialize core operators (cannot be changed globally)
        void initializeCoreOperators();

        // Map operator name to TokenType
        TokenType operatorNameToType(const std::string &name) const;
    };

    // Notation mode
    enum class NotationMode
    {
        MATH,   // Standard mathematical precedence (default)
        LINEAR, // All operators same precedence, left-assoc
        CUSTOM  // User-defined fixity
    };

    // Notation context (stack of precedence tables)
    class NotationContext
    {
    public:
        NotationContext();

        // Push a new notation scope
        void pushMath();                                         // Standard math
        void pushLinear();                                       // Left-to-right, no precedence
        void pushCustom(const std::string &name);                // Custom notation
        void pushCustom(std::unique_ptr<PrecedenceTable> table); // With explicit table

        // Pop notation scope
        void pop();

        // Get current precedence table
        const PrecedenceTable &current() const;
        PrecedenceTable &currentMutable();

        // Check if in linear mode
        bool isLinearMode() const;

        // Get current mode
        NotationMode currentMode() const;

    private:
        struct NotationScope
        {
            NotationMode mode;
            std::unique_ptr<PrecedenceTable> table;
            std::string name; // For custom notations

            NotationScope(NotationMode m, std::unique_ptr<PrecedenceTable> t, const std::string &n = "")
                : mode(m), table(std::move(t)), name(n) {}
        };

        std::vector<NotationScope> scopes;

        // Create table for given mode
        std::unique_ptr<PrecedenceTable> createTableForMode(NotationMode mode);
    };

} // namespace lpp

#endif // PRECEDENCE_TABLE_H
