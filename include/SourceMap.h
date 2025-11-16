#ifndef SOURCE_MAP_H
#define SOURCE_MAP_H

#include <string>
#include <vector>
#include <map>

namespace lpp
{

    struct SourceMapping
    {
        int lppLine;
        int lppColumn;
        int cppLine;
        int cppColumn;
        std::string sourceName;
    };

    class SourceMapGenerator
    {
    public:
        void addMapping(int lppLine, int lppCol, int cppLine, int cppCol, const std::string &source = "");

        void generateSourceMap(const std::string &outputPath);
        void generateDebugInfo(const std::string &outputPath);

        std::string getSourceMapJSON();

    private:
        std::vector<SourceMapping> mappings;
        std::string encodeVLQ(int value);
        std::string encodeBase64VLQ(const std::vector<int> &values);
    };

} // namespace lpp

#endif // SOURCE_MAP_H
