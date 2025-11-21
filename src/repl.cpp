#include "Lexer.h"
#include "Parser.h"
#include "Transpiler.h"
#include "StaticAnalyzer.h"
#include <iostream>
#include <string>
#include <sstream>
#include <cstdlib>

namespace lpp
{

    class REPL
    {
    public:
        void run();

    private:
        std::vector<std::string> history;
        std::stringstream context;
        int lineNumber = 1;

        std::string readLine();
        bool isComplete(const std::string &code);
        void evaluateExpression(const std::string &code);
        void evaluateStatement(const std::string &code);
        void printHelp();
    };

    void REPL::run()
    {
        std::cout << "L++ REPL v0.1\n";
        std::cout << "Type 'help' for help, 'exit' to quit\n\n";

        std::string input;
        std::string multiline;

        while (true)
        {
            if (multiline.empty())
            {
                std::cout << "lpp> ";
            }
            else
            {
                std::cout << "...> ";
            }

            std::string line = readLine();

            if (line == "exit" || line == "quit")
            {
                break;
            }

            if (line == "help")
            {
                printHelp();
                continue;
            }

            if (line == "clear")
            {
                // BUG #345 fix: Use safe platform-specific commands
#ifdef _WIN32
                system("cls");
#else
                system("clear");
#endif
                continue;
            }

            if (line == "history")
            {
                for (size_t i = 0; i < history.size(); i++)
                {
                    std::cout << i + 1 << ": " << history[i] << "\n";
                }
                continue;
            }

            multiline += line + "\n";

            if (isComplete(multiline))
            {
                history.push_back(multiline);

                try
                {
                    // Wrap in paradigm and main
                    std::string fullCode = "#pragma paradigm hybrid\n\n";
                    fullCode += "fn main() -> int {\n";
                    fullCode += multiline;
                    fullCode += "\n    return 0;\n}\n";

                    // Lex
                    Lexer lexer(fullCode);
                    auto tokens = lexer.tokenize();

                    // Parse
                    Parser parser(tokens, fullCode);
                    auto ast = parser.parse();

                    if (parser.hasErrors())
                    {
                        for (const auto &err : parser.getErrors())
                        {
                            std::cerr << err;
                        }
                        multiline.clear();
                        continue;
                    }

                    // Transpile
                    Transpiler transpiler;
                    std::string cpp = transpiler.transpile(*ast);

                    std::cout << "[Transpiled successfully]\n";
                }
                catch (const std::exception &e)
                {
                    std::cerr << "Error: " << e.what() << "\n";
                }

                multiline.clear();
            }
        }

        std::cout << "\nGoodbye!\n";
    }

    std::string REPL::readLine()
    {
        std::string line;
        std::getline(std::cin, line);
        return line;
    }

    bool REPL::isComplete(const std::string &code)
    {
        // Simple heuristic: check for balanced braces
        int braces = 0;
        int parens = 0;
        int brackets = 0;

        for (char c : code)
        {
            if (c == '{')
                braces++;
            else if (c == '}')
                braces--;
            else if (c == '(')
                parens++;
            else if (c == ')')
                parens--;
            else if (c == '[')
                brackets++;
            else if (c == ']')
                brackets--;
        }

        // If ends with semicolon and balanced, it's complete
        if (braces == 0 && parens == 0 && brackets == 0)
        {
            std::string trimmed = code;
            // FIX BUG #350: Check npos before erase to prevent UB
            size_t lastNonSpace = trimmed.find_last_not_of(" \n\r\t");
            if (lastNonSpace != std::string::npos)
            {
                trimmed.erase(lastNonSpace + 1);
                if (!trimmed.empty() && trimmed.back() == ';')
                {
                    return true;
                }
            }
        }

        return false;
    }

    void REPL::printHelp()
    {
        std::cout << "\nL++ REPL Commands:\n";
        std::cout << "  help     - Show this help message\n";
        std::cout << "  exit     - Exit the REPL\n";
        std::cout << "  quit     - Exit the REPL\n";
        std::cout << "  clear    - Clear the screen\n";
        std::cout << "  history  - Show command history\n";
        std::cout << "\nEnter L++ code and press Enter. Multi-line input is supported.\n\n";
    }

} // namespace lpp

int main()
{
    lpp::REPL repl;
    repl.run();
    return 0;
}
