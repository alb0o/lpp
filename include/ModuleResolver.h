#ifndef MODULE_RESOLVER_H
#define MODULE_RESOLVER_H

#include <string>
#include <vector>
#include <set>
#include <map>
#include <filesystem>

namespace lpp
{

    class ModuleResolver
    {
    public:
        ModuleResolver() : currentFilePath("."), currentDirectory(".") {}
        ModuleResolver(const std::string &currentFile);

        // Resolve import path to absolute file path
        std::string resolve(const std::string &importPath);

        // Check for circular dependencies
        bool hasCircularDependency(const std::string &modulePath);

        // Mark module as being imported
        void addDependency(const std::string &from, const std::string &to);

        // Get all dependencies of a module
        std::vector<std::string> getDependencies(const std::string &modulePath);

        // Get error messages
        const std::vector<std::string> &getErrors() const { return errors; }
        bool hasErrors() const { return !errors.empty(); }

    private:
        std::string currentFilePath;
        std::string currentDirectory;
        std::map<std::string, std::vector<std::string>> dependencyGraph;
        std::vector<std::string> errors;

        // Helper functions
        std::string resolveRelative(const std::string &path);
        std::string resolveAbsolute(const std::string &path);
        bool fileExists(const std::string &path);
        bool detectCycle(const std::string &module, std::set<std::string> &visited, std::set<std::string> &recursionStack);
    };

} // namespace lpp

#endif // MODULE_RESOLVER_H
