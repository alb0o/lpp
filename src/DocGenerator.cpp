#include "DocGenerator.h"
#include <iostream>
#include <sstream>

namespace lpp
{

    DocGenerator::DocGenerator(const std::string &path)
        : outputPath(path)
    {
        markdown << "# L++ Documentation\n\n";
        markdown << "Auto-generated API documentation.\n\n";
    }

    void DocGenerator::generate(const Program &ast)
    {
        // Functions
        if (!ast.functions.empty())
        {
            markdown << "## Functions\n\n";
            for (const auto &fn : ast.functions)
            {
                generateFunction(*fn);
            }
        }

        // Classes
        if (!ast.classes.empty())
        {
            markdown << "## Classes\n\n";
            for (const auto &cls : ast.classes)
            {
                generateClass(*cls);
            }
        }

        // Interfaces
        if (!ast.interfaces.empty())
        {
            markdown << "## Interfaces\n\n";
            for (const auto &iface : ast.interfaces)
            {
                generateInterface(*iface);
            }
        }
    }

    void DocGenerator::generateFunction(const Function &fn, const DocComment *comment)
    {
        markdown << "### `" << fn.name << "`\n\n";

        if (comment && !comment->description.empty())
        {
            markdown << comment->description << "\n\n";
        }

        // Signature
        markdown << "**Signature:**\n```lpp\n";
        if (fn.isAsync)
            markdown << "async ";
        markdown << "fn " << fn.name;

        if (!fn.genericParams.empty())
        {
            markdown << "<";
            for (size_t i = 0; i < fn.genericParams.size(); i++)
            {
                markdown << fn.genericParams[i];
                if (i < fn.genericParams.size() - 1)
                    markdown << ", ";
            }
            markdown << ">";
        }

        markdown << "(";
        for (size_t i = 0; i < fn.parameters.size(); i++)
        {
            markdown << fn.parameters[i].first << ": " << fn.parameters[i].second;
            if (i < fn.parameters.size() - 1)
                markdown << ", ";
        }

        if (fn.hasRestParam)
        {
            if (!fn.parameters.empty())
                markdown << ", ";
            markdown << "..." << fn.restParamName;
        }

        markdown << ") -> " << fn.returnType << "\n```\n\n";

        // Parameters
        if (!fn.parameters.empty() || fn.hasRestParam)
        {
            markdown << "**Parameters:**\n";
            for (const auto &param : fn.parameters)
            {
                markdown << "- `" << param.first << "` (" << param.second << ")";
                if (comment)
                {
                    for (const auto &p : comment->params)
                    {
                        if (p.first == param.first)
                        {
                            markdown << " - " << p.second;
                            break;
                        }
                    }
                }
                markdown << "\n";
            }
            if (fn.hasRestParam)
            {
                markdown << "- `..." << fn.restParamName << "` (variadic) - Rest parameters\n";
            }
            markdown << "\n";
        }

        // Returns
        markdown << "**Returns:** `" << fn.returnType << "`";
        if (comment && !comment->returns.empty())
        {
            markdown << " - " << comment->returns;
        }
        markdown << "\n\n";

        // Examples
        if (comment && !comment->examples.empty())
        {
            markdown << "**Examples:**\n```lpp\n";
            for (const auto &example : comment->examples)
            {
                markdown << example << "\n";
            }
            markdown << "```\n\n";
        }

        markdown << "---\n\n";
    }

    void DocGenerator::generateClass(const ClassDecl &cls, const DocComment *comment)
    {
        markdown << "### `class " << cls.name << "`\n\n";

        if (comment && !comment->description.empty())
        {
            markdown << comment->description << "\n\n";
        }

        if (!cls.baseClass.empty())
        {
            markdown << "**Extends:** `" << cls.baseClass << "`\n\n";
        }

        // Properties
        if (!cls.properties.empty())
        {
            markdown << "**Properties:**\n";
            for (const auto &prop : cls.properties)
            {
                markdown << "- `" << prop.first << ": " << prop.second << "`\n";
            }
            markdown << "\n";
        }

        // Methods
        if (!cls.methods.empty())
        {
            markdown << "**Methods:**\n";
            for (const auto &method : cls.methods)
            {
                markdown << "- `" << method->name << "(...)";
                markdown << " -> " << method->returnType << "`\n";
            }
            markdown << "\n";
        }

        markdown << "---\n\n";
    }

    void DocGenerator::generateInterface(const InterfaceDecl &iface, const DocComment *comment)
    {
        markdown << "### `interface " << iface.name << "`\n\n";

        if (comment && !comment->description.empty())
        {
            markdown << comment->description << "\n\n";
        }

        markdown << "**Methods:**\n";
        for (const auto &method : iface.methods)
        {
            markdown << "- `" << method.first << "` - " << method.second << "\n";
        }
        markdown << "\n---\n\n";
    }

    void DocGenerator::writeToFile()
    {
        std::ofstream file(outputPath);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open documentation output file: " << outputPath << std::endl;
            return;
        }
        file << markdown.str();
        file.close();
        std::cout << "Documentation generated: " << outputPath << std::endl;
    }

    std::string DocGenerator::typeToString(const std::string &type)
    {
        return type;
    }

    std::string DocGenerator::escapeMarkdown(const std::string &text)
    {
        // Simple escaping for markdown special characters
        std::string result = text;
        // Add escaping logic if needed
        return result;
    }

    DocComment DocGenerator::parseDocComment(const std::string &comment)
    {
        DocComment doc;
        // Parse JSDoc-style comments
        // For now, return empty
        return doc;
    }

} // namespace lpp
