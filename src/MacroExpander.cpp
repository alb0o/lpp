#include "MacroExpander.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace lpp
{

    void MacroExpander::defineMacro(const std::string &name, const std::string &body)
    {
        MacroDefinition macro;
        macro.name = name;
        macro.body = body;
        macro.isFunction = false;
        macros[name] = macro;
    }

    void MacroExpander::defineFunctionMacro(const std::string &name, const std::vector<std::string> &params, const std::string &body)
    {
        MacroDefinition macro;
        macro.name = name;
        macro.parameters = params;
        macro.body = body;
        macro.isFunction = true;
        macros[name] = macro;
    }

    std::string MacroExpander::expand(const std::string &source)
    {
        std::string result = source;
        const int MAX_EXPANSION_DEPTH = 100;
        int expansionCount = 0;

        bool changed = true;
        while (changed && expansionCount < MAX_EXPANSION_DEPTH)
        {
            changed = false;
            std::string previous = result;

            // Simple token-based expansion
            for (const auto &pair : macros)
            {
                const std::string &name = pair.first;
                const MacroDefinition &macro = pair.second;

                if (!macro.isFunction)
                {
                    // Object-like macro - simple replacement
                    size_t pos = 0;
                    while ((pos = result.find(name, pos)) != std::string::npos)
                    {
                        // Check if it's a whole word
                        bool isWord = true;
                        if (pos > 0 && (isalnum(result[pos - 1]) || result[pos - 1] == '_'))
                        {
                            isWord = false;
                        }
                        if (pos + name.length() < result.length() &&
                            (isalnum(result[pos + name.length()]) || result[pos + name.length()] == '_'))
                        {
                            isWord = false;
                        }

                        if (isWord)
                        {
                            result.replace(pos, name.length(), macro.body);
                            pos += macro.body.length();
                            changed = true;
                        }
                        else
                        {
                            pos++;
                        }
                    }
                }
            }

            expansionCount++;
        }

        if (expansionCount >= MAX_EXPANSION_DEPTH)
        {
            // Warn about possible recursive macro
            std::cerr << "Warning: Macro expansion depth limit reached (" << MAX_EXPANSION_DEPTH << "). Possible recursive macro.\n";
        }

        return result;
    }

    bool MacroExpander::hasMacro(const std::string &name) const
    {
        return macros.find(name) != macros.end();
    }

    void MacroExpander::addBuiltins()
    {
        // Add common built-in macros
        defineMacro("__LPP__", "1");
        defineMacro("__VERSION__", "\"0.1.0\"");

// Platform macros
#ifdef _WIN32
        defineMacro("__WINDOWS__", "1");
#elif defined(__linux__)
        defineMacro("__LINUX__", "1");
#elif defined(__APPLE__)
        defineMacro("__MACOS__", "1");
#endif
    }

    std::string MacroExpander::expandMacro(const std::string &name, const std::vector<std::string> &args)
    {
        if (!hasMacro(name))
        {
            return "";
        }

        const MacroDefinition &macro = macros[name];

        if (!macro.isFunction)
        {
            return macro.body;
        }

        // Substitute parameters
        std::string result = macro.body;
        for (size_t i = 0; i < macro.parameters.size() && i < args.size(); i++)
        {
            const std::string &param = macro.parameters[i];
            const std::string &arg = args[i];

            size_t pos = 0;
            while ((pos = result.find(param, pos)) != std::string::npos)
            {
                result.replace(pos, param.length(), arg);
                pos += arg.length();
            }
        }

        return result;
    }

    std::vector<std::string> MacroExpander::parseArguments(const std::string &text, size_t &pos)
    {
        std::vector<std::string> args;
        // Simple argument parsing
        // Skip to implementation when needed
        return args;
    }

} // namespace lpp
