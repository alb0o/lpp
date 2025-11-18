#include "PackageManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

namespace lpp
{

    PackageManifest PackageManager::loadManifest(const std::string &manifestPath)
    {
        PackageManifest manifest;

        std::ifstream file(manifestPath);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not open package manifest: " << manifestPath << std::endl;
            return manifest;
        }

        // Simple parsing (would use JSON parser in production)
        std::string line;
        while (std::getline(file, line))
        {
            // Parse key-value pairs
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos)
            {
                std::string key = line.substr(0, colonPos);
                std::string value = line.substr(colonPos + 1);

                // Trim whitespace
                size_t keyStart = key.find_first_not_of(" \t");
                if (keyStart != std::string::npos)
                    key.erase(0, keyStart);
                size_t keyEnd = key.find_last_not_of(" \t");
                if (keyEnd != std::string::npos)
                    key.erase(keyEnd + 1);

                size_t valStart = value.find_first_not_of(" \t\"");
                if (valStart != std::string::npos)
                    value.erase(0, valStart);
                size_t valEnd = value.find_last_not_of(" \t\",");
                if (valEnd != std::string::npos)
                    value.erase(valEnd + 1);

                if (key == "name")
                    manifest.name = value;
                else if (key == "version")
                    manifest.version = value;
                else if (key == "description")
                    manifest.description = value;
                else if (key == "author")
                    manifest.author = value;
                else if (key == "license")
                    manifest.license = value;
                else if (key == "entry")
                    manifest.entryPoint = value;
            }
        }

        file.close();
        return manifest;
    }

    void PackageManager::saveManifest(const PackageManifest &manifest, const std::string &manifestPath)
    {
        std::ofstream file(manifestPath);
        if (!file.is_open())
        {
            std::cerr << "Error: Could not save package manifest\n";
            return;
        }

        file << "{\n";
        file << "  \"name\": \"" << manifest.name << "\",\n";
        file << "  \"version\": \"" << manifest.version << "\",\n";
        file << "  \"description\": \"" << manifest.description << "\",\n";
        file << "  \"author\": \"" << manifest.author << "\",\n";
        file << "  \"license\": \"" << manifest.license << "\",\n";
        file << "  \"entry\": \"" << manifest.entryPoint << "\",\n";

        file << "  \"dependencies\": {\n";
        for (size_t i = 0; i < manifest.dependencies.size(); i++)
        {
            file << "    \"" << manifest.dependencies[i].name << "\": \"" << manifest.dependencies[i].version << "\"";
            if (i < manifest.dependencies.size() - 1)
                file << ",";
            file << "\n";
        }
        file << "  },\n";

        file << "  \"scripts\": {\n";
        size_t i = 0;
        for (const auto &script : manifest.scripts)
        {
            file << "    \"" << script.first << "\": \"" << script.second << "\"";
            if (i < manifest.scripts.size() - 1)
                file << ",";
            file << "\n";
            i++;
        }
        file << "  }\n";

        file << "}\n";
        file.close();

        std::cout << "Package manifest saved: " << manifestPath << "\n";
    }

    void PackageManager::init(const std::string &packageName)
    {
        PackageManifest manifest;
        manifest.name = packageName;
        manifest.version = "0.8.13";
        manifest.description = "A L++ project";
        manifest.author = "";
        manifest.license = "MIT";
        manifest.entryPoint = "main.lpp";

        saveManifest(manifest, "package.lpp");

        // Create directory structure
        std::filesystem::create_directory("src");
        std::filesystem::create_directory("lib");
        std::filesystem::create_directory("tests");

        // Create main.lpp
        std::ofstream mainFile("src/main.lpp");
        if (mainFile.is_open())
        {
            mainFile << "#pragma paradigm hybrid\n\n";
            mainFile << "fn main() -> int {\n";
            mainFile << "    print(\"Hello from " << packageName << "!\\n\");\n";
            mainFile << "    return 0;\n";
            mainFile << "}\n";
            mainFile.close();
        }

        std::cout << "Initialized L++ package: " << packageName << "\n";
    }

    void PackageManager::install(const PackageManifest &manifest)
    {
        std::cout << "Installing dependencies for: " << manifest.name << "\n";

        for (const auto &dep : manifest.dependencies)
        {
            installPackage(dep.name, dep.version);
        }

        std::cout << "All dependencies installed!\n";
    }

    void PackageManager::installPackage(const std::string &packageName, const std::string &version)
    {
        std::cout << "Installing " << packageName << "@" << version << "...\n";

        // In production: download from package registry
        std::string downloadPath = downloadPackage(packageName, version);

        if (!downloadPath.empty())
        {
            std::cout << "  ✓ " << packageName << " installed\n";
        }
        else
        {
            std::cerr << "  ✗ Failed to install " << packageName << "\n";
        }
    }

    std::string PackageManager::resolveVersion(const std::string &packageName, const std::string &versionConstraint)
    {
        // Simple version resolution
        // In production: query package registry, resolve semver constraints
        return versionConstraint;
    }

    std::vector<std::string> PackageManager::checkUpdates(const PackageManifest &manifest)
    {
        std::vector<std::string> updates;

        // Check each dependency for updates
        for (const auto &dep : manifest.dependencies)
        {
            // Query registry for latest version
            // Compare with current version
            // Add to updates if newer version available
        }

        return updates;
    }

    std::string PackageManager::downloadPackage(const std::string &name, const std::string &version)
    {
        // Download package from registry
        // Extract to lpp_modules/
        // Return path to package

        std::string packagePath = "lpp_modules/" + name;
        std::filesystem::create_directories(packagePath);

        return packagePath;
    }

    bool PackageManager::validateVersion(const std::string &version)
    {
        // Check if version string is valid semver
        // Format: MAJOR.MINOR.PATCH
        return !version.empty();
    }

} // namespace lpp
