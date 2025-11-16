#include "SourceMap.h"
#include <fstream>
#include <sstream>
#include <algorithm>

namespace lpp
{

    void SourceMapGenerator::addMapping(int lppLine, int lppCol, int cppLine, int cppCol, const std::string &source)
    {
        SourceMapping mapping;
        mapping.lppLine = lppLine;
        mapping.lppColumn = lppCol;
        mapping.cppLine = cppLine;
        mapping.cppColumn = cppCol;
        mapping.sourceName = source;
        mappings.push_back(mapping);
    }

    void SourceMapGenerator::generateSourceMap(const std::string &outputPath)
    {
        std::ofstream file(outputPath);
        if (!file.is_open())
        {
            return;
        }

        file << getSourceMapJSON();
        file.close();
    }

    void SourceMapGenerator::generateDebugInfo(const std::string &outputPath)
    {
        std::ofstream file(outputPath);
        if (!file.is_open())
        {
            return;
        }

        file << "# L++ Debug Information\n\n";
        file << "## Line Mappings (L++ -> C++)\n\n";

        for (const auto &mapping : mappings)
        {
            file << "L++ " << mapping.lppLine << ":" << mapping.lppColumn;
            file << " -> C++ " << mapping.cppLine << ":" << mapping.cppColumn;
            if (!mapping.sourceName.empty())
            {
                file << " (" << mapping.sourceName << ")";
            }
            file << "\n";
        }

        file.close();
    }

    std::string SourceMapGenerator::getSourceMapJSON()
    {
        std::stringstream json;
        json << "{\n";
        json << "  \"version\": 3,\n";
        json << "  \"file\": \"output.cpp\",\n";
        json << "  \"sourceRoot\": \"\",\n";
        json << "  \"sources\": [\"source.lpp\"],\n";
        json << "  \"names\": [],\n";
        json << "  \"mappings\": \"";

        // Simplified mappings - full VLQ encoding would be complex
        for (size_t i = 0; i < mappings.size(); i++)
        {
            if (i > 0)
                json << ";";
            json << "AAAA"; // Placeholder
        }

        json << "\"\n";
        json << "}\n";

        return json.str();
    }

    std::string SourceMapGenerator::encodeVLQ(int value)
    {
        // VLQ encoding for source maps
        // Simplified implementation
        return "A";
    }

    std::string SourceMapGenerator::encodeBase64VLQ(const std::vector<int> &values)
    {
        std::string result;
        for (int val : values)
        {
            result += encodeVLQ(val);
        }
        return result;
    }

} // namespace lpp
