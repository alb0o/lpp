#ifndef FFI_H
#define FFI_H

#include <string>
#include <vector>
#include <map>

namespace lpp
{

    // FIX BUG #171: FFI boundary doesn't validate C++ resource cleanup
    // TODO: Add RAII wrapper for FFI handles
    // - FFIHandle<T> class: Owns foreign resource, auto-cleanup in destructor
    // - Track open handles: std::set<void*> openHandles;
    // - On exit: assert(openHandles.empty()) // All handles closed
    // - Detect leaks: "FFI handle 0x1234 not released"
    // Example:
    //   class FFIHandle {
    //     void* handle;
    //   public:
    //     ~FFIHandle() { if (handle) closeFFI(handle); }
    //   };
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
