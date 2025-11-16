#include "FFI.h"
#include <sstream>

namespace lpp
{

    void FFIGenerator::addExternFunction(const ExternFunction &func)
    {
        externFunctions.push_back(func);
    }

    std::string FFIGenerator::generateBindings()
    {
        std::stringstream code;

        code << "// FFI Bindings - Auto-generated\n";
        code << "#include <dlfcn.h>  // For dynamic loading\n\n";

        for (const auto &func : externFunctions)
        {
            code << generateCppWrapper(func);
        }

        return code.str();
    }

    std::string FFIGenerator::generateHeader()
    {
        std::stringstream header;

        header << "#ifndef LPP_FFI_H\n";
        header << "#define LPP_FFI_H\n\n";

        for (const auto &func : externFunctions)
        {
            if (func.isCFunction)
            {
                header << "extern \"C\" ";
            }
            header << mapLppTypeToCpp(func.returnType) << " " << func.name << "(";

            for (size_t i = 0; i < func.parameters.size(); i++)
            {
                header << mapLppTypeToCpp(func.parameters[i].second) << " " << func.parameters[i].first;
                if (i < func.parameters.size() - 1)
                {
                    header << ", ";
                }
            }

            header << ");\n";
        }

        header << "\n#endif // LPP_FFI_H\n";

        return header.str();
    }

    std::string FFIGenerator::generateCppWrapper(const ExternFunction &func)
    {
        std::stringstream wrapper;

        wrapper << "// Wrapper for: " << func.name << "\n";

        if (func.isCFunction)
        {
            wrapper << "extern \"C\" {\n";
        }

        wrapper << "    " << mapLppTypeToCpp(func.returnType) << " " << func.name << "(";

        for (size_t i = 0; i < func.parameters.size(); i++)
        {
            wrapper << mapLppTypeToCpp(func.parameters[i].second) << " " << func.parameters[i].first;
            if (i < func.parameters.size() - 1)
            {
                wrapper << ", ";
            }
        }

        wrapper << ");\n";

        if (func.isCFunction)
        {
            wrapper << "}\n";
        }

        wrapper << "\n";

        return wrapper.str();
    }

    std::string FFIGenerator::mapLppTypeToCpp(const std::string &lppType)
    {
        if (lppType == "int")
            return "int";
        if (lppType == "float")
            return "double";
        if (lppType == "string")
            return "const char*";
        if (lppType == "bool")
            return "bool";
        if (lppType == "void")
            return "void";
        return lppType; // Default: use as-is
    }

} // namespace lpp
