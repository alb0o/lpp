#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <algorithm>
#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include "StaticAnalyzer.h"

void printUsage(const char *programName)
{
    std::cout << "Usage: " << programName << " <input.lpp> [-o <output>]\n";
    std::cout << "Options:\n";
    std::cout << "  -o <output>   Specify output executable name (default: a.out)\n";
    std::cout << "  -c            Generate C++ only (no compilation)\n";
    std::cout << "  --help        Show this help message\n";
}

std::string readFile(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not open file '" << filename << "'\n";
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string &filename, const std::string &content)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Could not write to file '" << filename << "'\n";
        exit(1);
    }
    file << content;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputFile;
    std::string outputFile = "a.out";
    bool compileOnly = false;

    // Parse arguments
    for (int i = 1; i < argc; i++)
    {
        std::string arg = argv[i];

        if (arg == "--help")
        {
            printUsage(argv[0]);
            return 0;
        }
        else if (arg == "-o" && i + 1 < argc)
        {
            outputFile = argv[++i];
        }
        else if (arg == "-c")
        {
            compileOnly = true;
        }
        else if (inputFile.empty())
        {
            inputFile = arg;
        }
    }

    if (inputFile.empty())
    {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    std::cout << "LPP Compiler v0.1\n";
    std::cout << "Compiling: " << inputFile << "\n";

    // Read source code
    std::string source = readFile(inputFile);

    // Lexical analysis
    std::cout << "Lexing...\n";
    lpp::Lexer lexer(source);
    std::vector<lpp::Token> tokens = lexer.tokenize();

    // Parsing
    std::cout << "Parsing...\n";
    lpp::Parser parser(tokens, source); // Pass source code for better error messages
    std::unique_ptr<lpp::Program> ast = parser.parse();

    // Check for parse errors
    if (parser.hasErrors())
    {
        std::cerr << "\nParsing failed with " << parser.getErrors().size() << " error(s).\n";
        return 1;
    }

    // Static analysis
    std::cout << "Running static analysis...\n";
    lpp::StaticAnalyzer analyzer;
    std::vector<lpp::AnalysisIssue> issues = analyzer.analyze(*ast);

    // Report issues in standard compiler format
    int errorCount = 0;
    int warningCount = 0;

    for (const auto &issue : issues)
    {
        // Standard format: filename:line:column: severity: message
        std::cerr << inputFile << ":" << issue.line << ":" << issue.column << ": ";

        if (issue.severity == lpp::Severity::ERROR)
        {
            errorCount++;
            std::cerr << "error: ";
        }
        else if (issue.severity == lpp::Severity::WARNING)
        {
            warningCount++;
            std::cerr << "warning: ";
        }
        else
        {
            std::cerr << "note: ";
        }

        // Error code in brackets
        std::cerr << "[";
        switch (issue.type)
        {
        case lpp::IssueType::NULL_DEREFERENCE:
            std::cerr << "NULL-DEREF";
            break;
        case lpp::IssueType::DIVISION_BY_ZERO:
            std::cerr << "DIV-BY-ZERO";
            break;
        case lpp::IssueType::UNINITIALIZED_READ:
            std::cerr << "UNINIT-READ";
            break;
        case lpp::IssueType::INTEGER_OVERFLOW:
            std::cerr << "INT-OVERFLOW";
            break;
        case lpp::IssueType::MEMORY_LEAK:
            std::cerr << "MEM-LEAK";
            break;
        case lpp::IssueType::DEAD_CODE:
            std::cerr << "DEAD-CODE";
            break;
        case lpp::IssueType::BUFFER_OVERFLOW:
            std::cerr << "BUFFER-OVERFLOW";
            break;
        case lpp::IssueType::PARADIGM_MUTATION_IN_FUNCTIONAL:
            std::cerr << "PARADIGM-FUNC";
            break;
        case lpp::IssueType::PARADIGM_CLASS_IN_FUNCTIONAL:
            std::cerr << "PARADIGM-FUNC";
            break;
        case lpp::IssueType::PARADIGM_CLASS_IN_IMPERATIVE:
            std::cerr << "PARADIGM-IMP";
            break;
        case lpp::IssueType::PARADIGM_GOLF_DISCOURAGED:
            std::cerr << "PARADIGM-GOLF";
            break;
        case lpp::IssueType::PARADIGM_GOLF_ENCOURAGED:
            std::cerr << "PARADIGM-GOLF";
            break;
        default:
            std::cerr << "UNKNOWN";
        }
        std::cerr << "] ";

        // Message
        std::cerr << issue.message;

        if (!issue.function.empty())
        {
            std::cerr << " in function '" << issue.function << "'";
        }
        std::cerr << "\n";

        // Additional notes
        for (const auto &note : issue.notes)
        {
            std::cerr << inputFile << ":" << issue.line << ":" << issue.column
                      << ": note: " << note << "\n";
        }
    }

    if (errorCount > 0)
    {
        std::cerr << "\nAnalysis failed with " << errorCount << " error(s) and "
                  << warningCount << " warning(s)\n";
        return 1;
    }

    if (warningCount > 0)
    {
        std::cout << "\nAnalysis passed with " << warningCount << " warning(s)\n";
    }
    else
    {
        std::cout << "✓ Analysis passed with no issues\n";
    }

    // Transpilation
    std::cout << "Transpiling to C++...\n";
    lpp::Transpiler transpiler;
    std::string cppCode = transpiler.transpile(*ast);

    // Write generated C++ code
    std::string cppFile = inputFile + ".cpp";
    writeFile(cppFile, cppCode);
    std::cout << "Generated: " << cppFile << "\n";

    if (compileOnly)
    {
        std::cout << "Compilation skipped (-c flag)\n";
        return 0;
    }

    // Compile with g++ (or clang++)
    std::cout << "Compiling with g++...\n";

    // Sanitize path to prevent command injection
    std::string sanitizedOutput = outputFile;
    std::string sanitizedCpp = cppFile;
    // Remove potentially dangerous characters
    sanitizedOutput.erase(std::remove_if(sanitizedOutput.begin(), sanitizedOutput.end(),
                                         [](char c)
                                         { return c == '&' || c == '|' || c == ';' || c == '`' || c == '$'; }),
                          sanitizedOutput.end());
    sanitizedCpp.erase(std::remove_if(sanitizedCpp.begin(), sanitizedCpp.end(),
                                      [](char c)
                                      { return c == '&' || c == '|' || c == ';' || c == '`' || c == '$'; }),
                       sanitizedCpp.end());

    std::string safeCommand = "g++ " + sanitizedCpp + " -o " + sanitizedOutput + " -std=c++17";
    int result = system(safeCommand.c_str());

    if (result == 0)
    {
        std::cout << "Success! Executable: " << outputFile << "\n";
    }
    else
    {
        std::cerr << "Error: Compilation failed\n";
        return 1;
    }

    return 0;
}
