#ifndef FFI_H
#define FFI_H

#include <string>
#include <vector>
#include <map>

namespace lpp
{

    struct ExternFunction
    {
        std::string name;
        std::string returnType;
        std::vector<std::pair<std::string, std::string>> parameters;
        std::string libraryName;
        bool isCFunction; // extern "C" linkage
    };

    class FFIGenerator
    {
    public:
        void addExternFunction(const ExternFunction &func);

        std::string generateBindings();
        std::string generateHeader();
        std::string generateCppWrapper(const ExternFunction &func);

    private:
        std::vector<ExternFunction> externFunctions;

        std::string mapLppTypeToCpp(const std::string &lppType);
    };

} // namespace lpp

#endif // FFI_H
