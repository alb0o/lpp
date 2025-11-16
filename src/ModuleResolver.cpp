#include "ModuleResolver.h"
#include <iostream>
#include <algorithm>

namespace lpp
{

    ModuleResolver::ModuleResolver(const std::string &currentFile)
        : currentFilePath(currentFile)
    {
        // Extract directory from current file path
        std::filesystem::path path(currentFile);
        currentDirectory = path.parent_path().string();
        if (currentDirectory.empty())
        {
            currentDirectory = ".";
        }
    }

    std::string ModuleResolver::resolve(const std::string &importPath)
    {
        // Relative path: starts with ./ or ../
        if (importPath.find("./") == 0 || importPath.find("../") == 0)
        {
            return resolveRelative(importPath);
        }

        // Absolute path
        if (importPath[0] == '/' || (importPath.length() > 1 && importPath[1] == ':'))
        {
            return resolveAbsolute(importPath);
        }

        // Standard library or package
        // For now, treat as relative to stdlib directory
        std::string stdlibPath = "stdlib/" + importPath;
        if (fileExists(stdlibPath + ".lpp"))
        {
            return stdlibPath + ".lpp";
        }

        errors.push_back("Cannot resolve module: " + importPath);
        return "";
    }

    std::string ModuleResolver::resolveRelative(const std::string &path)
    {
        std::filesystem::path fullPath = std::filesystem::path(currentDirectory) / path;

        // Try with .lpp extension
        if (fileExists(fullPath.string() + ".lpp"))
        {
            return std::filesystem::canonical(fullPath.string() + ".lpp").string();
        }

        // Try as-is
        if (fileExists(fullPath.string()))
        {
            return std::filesystem::canonical(fullPath).string();
        }

        errors.push_back("Module not found: " + path);
        return "";
    }

    std::string ModuleResolver::resolveAbsolute(const std::string &path)
    {
        // Try with .lpp extension
        if (fileExists(path + ".lpp"))
        {
            return path + ".lpp";
        }

        // Try as-is
        if (fileExists(path))
        {
            return path;
        }

        errors.push_back("Module not found: " + path);
        return "";
    }

    bool ModuleResolver::fileExists(const std::string &path)
    {
        return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
    }

    void ModuleResolver::addDependency(const std::string &from, const std::string &to)
    {
        dependencyGraph[from].push_back(to);
    }

    std::vector<std::string> ModuleResolver::getDependencies(const std::string &modulePath)
    {
        if (dependencyGraph.find(modulePath) != dependencyGraph.end())
        {
            return dependencyGraph[modulePath];
        }
        return {};
    }

    bool ModuleResolver::hasCircularDependency(const std::string &modulePath)
    {
        std::set<std::string> visited;
        std::set<std::string> recursionStack;
        return detectCycle(modulePath, visited, recursionStack);
    }

    bool ModuleResolver::detectCycle(const std::string &module,
                                     std::set<std::string> &visited,
                                     std::set<std::string> &recursionStack)
    {
        if (recursionStack.find(module) != recursionStack.end())
        {
            // Found cycle!
            errors.push_back("Circular dependency detected involving: " + module);
            return true;
        }

        if (visited.find(module) != visited.end())
        {
            return false; // Already checked this path
        }

        visited.insert(module);
        recursionStack.insert(module);

        // Check all dependencies
        auto deps = getDependencies(module);
        for (const auto &dep : deps)
        {
            if (detectCycle(dep, visited, recursionStack))
            {
                return true;
            }
        }

        recursionStack.erase(module);
        return false;
    }

} // namespace lpp
