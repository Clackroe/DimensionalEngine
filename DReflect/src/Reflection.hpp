#ifndef REFLECTION_HPP
#define REFLECTION_HPP

#include <DataStructs.hpp>
#include <filesystem>
#include <string>
#include <vector>

struct Reflection {

    void parseFile(const std::string& filePath, const std::string& absSourcePath, std::vector<ClassMeta>& classes);
    void parseBody(const std::string& bodyText, ClassMeta& cls);

    void parseDirectory(const std::filesystem::path& directory, std::vector<ClassMeta>& classes);

    void writeGeneratedData(std::filesystem::path& output, const std::vector<ClassMeta>& classes);
};
#endif // REFLECTION_HPP
