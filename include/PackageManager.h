#ifndef PACKAGE_MANAGER_H
#define PACKAGE_MANAGER_H

#include <string>
#include <vector>
#include <map>

namespace lpp
{

    struct PackageDependency
    {
        std::string name;
        std::string version;
        bool isDev;
    };

    struct PackageManifest
    {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::string entryPoint;
        std::vector<PackageDependency> dependencies;
        std::vector<PackageDependency> devDependencies;
        std::map<std::string, std::string> scripts;
    };

    class PackageManager
    {
    public:
        // Load package.lpp manifest
        static PackageManifest loadManifest(const std::string &manifestPath);

        // Save package.lpp manifest
        static void saveManifest(const PackageManifest &manifest, const std::string &manifestPath);

        // Initialize new package
        static void init(const std::string &packageName);

        // Install dependencies
        static void install(const PackageManifest &manifest);
        static void installPackage(const std::string &packageName, const std::string &version);

        // Resolve dependency versions
        static std::string resolveVersion(const std::string &packageName, const std::string &versionConstraint);

        // Check for updates
        static std::vector<std::string> checkUpdates(const PackageManifest &manifest);

    private:
        static std::string downloadPackage(const std::string &name, const std::string &version);
        static bool validateVersion(const std::string &version);
    };

} // namespace lpp

#endif // PACKAGE_MANAGER_H
