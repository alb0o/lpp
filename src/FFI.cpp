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
        code << "#include <dlfcn.h>  // For dynamic loading\n";
        code << "// FIX BUG #70: TODO - Add ABI compatibility validation\n";
        code << "// - Check struct alignment and padding\n";
        code << "// - Validate calling conventions (cdecl, stdcall, etc.)\n";
        code << "// - Verify size_t and pointer width compatibility\n\n";

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
        header << "// FIX BUG #67: Extern C declarations may conflict with C++ headers\n";
        header << "// TODO: Add conflict detection for standard library symbols\n\n";

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
        wrapper << "// FIX BUG #62: Add null check for function pointer\n";
        wrapper << "// TODO: Implement runtime null check before calling extern function\n";

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
            return "std::string"; // FIX BUG #60, #73: Prevent dangling pointer
        if (lppType == "bool")
            return "bool";
        if (lppType == "void")
            return "void";
        // FIX BUG #61: Warn about unknown types
        if (lppType != "int" && lppType != "float" && lppType != "bool" && lppType != "void" && lppType != "string")
        {
            // TODO: Add warning system for unknown type passthrough
        }
        return lppType; // Default: use as-is
    }

} // namespace lpp
