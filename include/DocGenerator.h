#ifndef DOC_GENERATOR_H
#define DOC_GENERATOR_H

#include "AST.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

namespace lpp
{

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
