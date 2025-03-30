
#include <Reflection.hpp>
#include <ReflectionData.hpp>

#include <DataStructs.hpp>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>
#include <vector>

void Reflection::parseFile(const std::string& filePath, const std::string& absSourcePath, std::vector<ClassMeta>& classes)
{
    std::ifstream file(filePath);

    if (!file.is_open()) {
        printf("ERROR: Failed to open the file: %s", filePath.c_str());
        return;
    }
    // This currently doestn support things other than classic class ClassName {

    std::regex classRegex(R"(\/\/\s*DM-CLASS\s*\n\s*class\s+(\w+))");

    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::smatch classMatch;
    std::string::const_iterator searchStart(content.cbegin());

    while (std::regex_search(searchStart, content.cend(), classMatch, classRegex)) {

        std::string className = classMatch[1].str();
        size_t classStartPosition = std::distance(content.cbegin(), classMatch[0].first);
        // printf("%s\n", className.c_str());

        // Finding the class body

        size_t openBracePosition = content.find("{", classStartPosition);

        if (openBracePosition == std::string::npos) {
            // Misformed class
            searchStart = classMatch[0].second;
            printf("Malformed Class, skipping");
            continue;
        }

        // Slow, but suitible
        int braceCount = 1;
        size_t closeBracePosition = openBracePosition + 1;

        while (braceCount > 0 && closeBracePosition < content.size()) {
            if (content[closeBracePosition] == '{')
                braceCount++;
            else if (content[closeBracePosition] == '}')
                braceCount--;
            closeBracePosition++;
        }

        if (braceCount != 0) {
            // Skip malformed class
            searchStart = classMatch[0].second;
            printf("Malformed Class, skipping");
            continue;
        }

        // Extract class body
        std::string classBody = content.substr(openBracePosition, openBracePosition - closeBracePosition);
        // printf("Body: \n%s\n", classBody.c_str());

        ClassMeta cls;
        cls.name = className;
        cls.path = std::filesystem::relative(std::filesystem::absolute(filePath), absSourcePath).string();
        parseBody(classBody, cls);

        classes.push_back(cls);

        searchStart = content.cbegin() + closeBracePosition;
    }
}

static bool isValidType(PropertyMeta prop)
{

    if (Dimensional::g_StringToScriptMember.contains(prop.type)) {
        return true;
    }
    return false;
};

static bool hasEvent(EVENTS ev, const std::string& bodyText)
{
    std::vector<std::string> matches;

    std::regex reg(R"(\bvoid\s+)" + g_EventsMap.at(ev) + R"(\s*\(\s*\)\s*\{)");

    std::sregex_iterator it(bodyText.begin(), bodyText.end(), reg);
    std::sregex_iterator end;
    while (it != end) {
        matches.push_back(it->str());
        ++it;
    }
    return matches.size() > 0;
};

void Reflection::parseBody(const std::string& bodyText, ClassMeta& cls)
{

    {
        if (hasEvent(EVENTS::UPDATE, bodyText)) {
            cls.hasUpdate = true;
        }
        if (hasEvent(EVENTS::DESTROY, bodyText)) {
            cls.hasDestroy = true;
        }
        if (hasEvent(EVENTS::CREATE, bodyText)) {
            cls.hasCreate = true;
        }
    }

    // Search for Properties
    {
        std::regex propRegex(R"(\/\/\s*DM-PROPERTY\s*\n\s*(\w+(?:::\w+)*(?:\s*[\*&])?)\s+(\w+)(?:\s*=\s*([^;]+))?\s*;)");
        std::smatch propMatch;
        std::string::const_iterator searchStart(bodyText.cbegin());

        while (std::regex_search(searchStart, bodyText.cend(), propMatch, propRegex)) {

            if (propMatch.size() > 2) {
                std::string propType = propMatch[1].str();
                std::string propName = propMatch[2].str();
                std::string propDefault = propMatch.size() > 3 ? propMatch[3].str() : "";

                propType = std::regex_replace(propType, std::regex(R"(^\s+|\s+$)"), "");
                propName = std::regex_replace(propName, std::regex(R"(^\s+|\s+$)"), "");
                propDefault = std::regex_replace(propDefault, std::regex(R"(^\s+|\s+$)"), "");

                PropertyMeta prop;
                prop.name = propName;
                prop.def = propDefault;
                prop.type = propType;

                if (isValidType(prop)) {
                    cls.props.push_back(prop);
                }
            }

            searchStart = propMatch[0].second;
        }
    }
}

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

void Reflection::parseDirectory(const std::filesystem::path& sourceDir, std::vector<ClassMeta>& classes)
{
    if (!std::filesystem::is_directory(sourceDir)) {
        printf("ERROR: Input path is not a directory: %s", sourceDir.c_str());
        return;
    }

    std::filesystem::path absSourcePath = std::filesystem::absolute(sourceDir);

    for (const auto& file : recursive_directory_iterator(sourceDir)) {
        if (file.is_directory()) {
            continue;
        }
        parseFile(file.path().string(), absSourcePath.string(), classes);
    }
}

static void writeCallableFunction(std::ofstream& stream, const std::string& className, const std::string& funcName, bool hasFunc)
{

    stream << "\t[](ScriptingCore::NativeScriptableEntity* pointer) {";

    if (hasFunc) {
        stream << "\n\t\t" << className << "* typedPTR = (" << className << "*)pointer;";
        stream << "\n\t\ttypedPTR->" << funcName << "();\n";
    }
    stream << "\t},\n\n";
}

static void writeGeneratedComment(std::ofstream& stream, const std::string& name)
{
    int offset = 11;
    int len = offset * 2 + name.length();

    stream << "\n//";
    for (int i = 0; i < len; i++) {
        stream << "=";
    };
    stream << "\n//";
    for (int i = 0; i < offset; i++) {
        stream << "=";
    };

    stream << name;

    for (int i = 0; i < offset; i++) {
        stream << "=";
    };
    stream << "\n//";
    for (int i = 0; i < len; i++) {
        stream << "=";
    };
}

static void writeMemberData(std::ofstream& stream, const PropertyMeta& prop, const ClassMeta& cls)
{

    stream << "\t\t//===========" << prop.name << "===========\n";

    stream << "\t\tMemberData {\n";

    ScriptMemberType type = Dimensional::g_StringToScriptMember.at(prop.type);

    stream << "\t\t\t.varName =\"" << prop.name << "\",\n";
    stream << "\t\t\t.defaultVal={." << Dimensional::g_ScriptMemberToMemberUnionName.at(type) << "=" << prop.def << "},\n";
    stream << "\t\t\t.dataType=" << Dimensional::g_StringToScriptMemberAsString.at(prop.type) << ",\n";

    stream << "\t\t\t.getter=[](ScriptingCore::NativeScriptableEntity* entity)-> void*{\n";
    stream << "\t\t\t\tauto* obj = (" << cls.name << "*)entity;\n";
    stream << "\t\t\t\treturn &(obj->" << prop.name << ");\n";
    stream << "\t\t\t},\n";

    stream << "\t\t\t.setter=[](ScriptingCore::NativeScriptableEntity* entity, void* value){\n";
    stream << "\t\t\t\tauto* obj = (" << cls.name << "*)entity;\n";
    stream << "\t\t\t\tobj->" << prop.name << "= *(" << prop.type << "*)value;\n";
    stream << "\t\t\t},\n";

    stream << "\t\t},\n";
}

static void writeClassData(std::ofstream& stream, const ClassMeta& cls)
{
    writeGeneratedComment(stream, cls.name);

    stream << "\n\nReflectedData {";

    stream << "\n\t .className = \"" << cls.name << "\",";
    stream << "\n\t .classFactory = [](uint64_t id) -> ScriptingCore::NativeScriptableEntity* { return new " << cls.name << "(id);},\n";

    stream << "\n\t .classDestructor = [](ScriptingCore::NativeScriptableEntity* pointer) {delete (" << cls.name << "*)pointer; },\n";

    stream << "\n\t .onUpdate = ";
    writeCallableFunction(stream, cls.name, g_EventsMap.at(EVENTS::UPDATE), cls.hasUpdate);

    stream << "\t .onCreate = ";
    writeCallableFunction(stream, cls.name, g_EventsMap.at(EVENTS::CREATE), cls.hasCreate);

    stream << "\t .onDestroy = ";
    writeCallableFunction(stream, cls.name, g_EventsMap.at(EVENTS::DESTROY), cls.hasDestroy);

    stream << "\t.memberData = {\n";

    for (auto prop : cls.props) {
        writeMemberData(stream, prop, cls);
    }

    stream << "\t}";

    stream << "},";
}

static void writeInitializeFunc(std::ofstream& stream)
{
    stream << "\nDM_GAMEAPI NativeScriptRegistry* Initialize(EngineAPI* eAPI, ComponentAPI* compAPI){\n";
    stream << "\tScriptCoreLink::Init(eAPI, compAPI);\n";
    stream << "\treturn &registry;\n";
    stream << "\n};\n";
}

static void writeCleanupFunc(std::ofstream& stream)
{
    stream << "\nDM_GAMEAPI void Cleanup(){\n";
    stream << "\tScriptCoreLink::ShutDown();\n";
    stream << "\n};\n";
}

void Reflection::writeGeneratedData(std::filesystem::path& output, const std::vector<ClassMeta>& classes)
{

    if (std::filesystem::exists(output)) {
        std::filesystem::remove(output);
    }

    std::ofstream file;
    file.open(output.string(), std::ios::out);

    if (!file.is_open()) {
        printf("Failed to open file: %s", output.c_str());
        return;
    }

    file << "#if defined(_WIN32) || defined(_WIN64)\n";
    file << "#define DM_WINDOWS\n";
    file << "#define DM_PLATFORM \"WINDOWS\"\n";
    file << "#define DM_GAMEAPI extern \"C\" __declspec(dllexport)\n";
    file << "#elif defined(__linux__)\n";
    file << "#define DM_LINUX\n";
    file << "#define DM_PLATFORM \"LINUX\"\n";
    file << "#define DM_GAMEAPI extern \"C\"";
    file << "\n#endif\n\n";

    file << "#include <ReflectionData.hpp>\n\n";

    file << "//==================ClassIncludes==================\n";
    for (auto cls : classes) {
        file << "#include <" << cls.path << ">\n";
    }

    file << "//==================ReflectedData==================\n";

    file << "NativeScriptRegistry registry = {\n";
    // =====

    for (auto cls : classes) {
        file << "\n{\"" << cls.name << "\"\n,";
        writeClassData(file, cls);
        file << "\n},";
    }
    // =====
    file << "\n};\n\n";

    writeInitializeFunc(file);
    writeCleanupFunc(file);
    file.close();
}
