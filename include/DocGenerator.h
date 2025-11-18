#ifndef DOC_GENERATOR_H
#define DOC_GENERATOR_H

#include "AST.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace lpp
{

    // FIX BUG #177: DocGenerator file handle may leak if write fails
    // TODO: Use std::ofstream RAII, ensure close() in all paths
    // - std::ofstream is RAII: Auto-closes in destructor
    // - But: Check is_open() before write
    // - Error handling: if (!out.is_open()) return error;
    // - No manual close() needed (destructor handles it)
    // - Consider: std::unique_ptr<std::ofstream> for explicit ownership
    struct DocComment
    {
        std::string description;
        std::vector<std::pair<std::string, std::string>> params; // name, description
        std::string returns;
        std::vector<std::string> examples;
    };

    class DocGenerator
    {
    public:
        explicit DocGenerator(const std::string &outputPath);

        void generate(const Program &ast);
        void generateFunction(const Function &fn, const DocComment *comment = nullptr);
        void generateClass(const ClassDecl &cls, const DocComment *comment = nullptr);
        void generateInterface(const InterfaceDecl &iface, const DocComment *comment = nullptr);

        void writeToFile();

    private:
        std::string outputPath;
        std::stringstream markdown;

        std::string typeToString(const std::string &type);
        std::string escapeMarkdown(const std::string &text);
        DocComment parseDocComment(const std::string &comment);
    };

} // namespace lpp

#endif // DOC_GENERATOR_H
