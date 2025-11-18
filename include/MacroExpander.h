#ifndef MACRO_EXPANDER_H
#define MACRO_EXPANDER_H

#include "AST.h"
#include <string>
#include <map>
#include <vector>

namespace lpp
{

    // FIX BUG #174: MacroExpander temp AST nodes may leak on expansion error
    // TODO: Use unique_ptr for all temporary AST allocations
    // - Store temp nodes in vector during expansion
    // - On error: vector auto-deletes all temps (RAII)
    // - On success: std::move temps to final AST
    // Example:
    //   std::vector<std::unique_ptr<ASTNode>> tempNodes;
    //   tempNodes.push_back(std::make_unique<Expr>());
    //   // On exception: tempNodes auto-cleanup
    struct MacroDefinition
    {
        std::string name;
        std::vector<std::string> parameters;
        std::string body;
        bool isFunction; // function-like vs object-like macro
    };

    class MacroExpander
    {
    public:
        void defineMacro(const std::string &name, const std::string &body);
        void defineFunctionMacro(const std::string &name, const std::vector<std::string> &params, const std::string &body);

        std::string expand(const std::string &source);
        bool hasMacro(const std::string &name) const;

        // Predefined macros
        void addBuiltins();

    private:
        std::map<std::string, MacroDefinition> macros;

        std::string expandMacro(const std::string &name, const std::vector<std::string> &args);
        std::vector<std::string> parseArguments(const std::string &text, size_t &pos);
    };

} // namespace lpp

#endif // MACRO_EXPANDER_H
