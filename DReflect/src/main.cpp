

#include <Reflection.hpp>
int main(int argc, char* argv[])
{

    std::string path;
    if (argc > 1) {
        path = argv[1];
    } else {
        printf("Please input a path: DReflect path/to/source/dir\n");
        return -1;
    }
    Reflection ref;
    std::vector<ClassMeta> classes;
    ref.parseDirectory(path, classes);

    for (auto cls : classes) {

        printf("Name: %s\n", cls.name.c_str());
        printf("\tPath: %s\n", cls.path.c_str());
        printf("\tUpdate: %b\n", cls.hasUpdate);
        printf("\tDestroy: %b\n", cls.hasDestroy);
        printf("\tCreate: %b\n", cls.hasCreate);

        for (auto prop : cls.props) {
            printf("\tName: %s\n", prop.name.c_str());
            printf("\tType: %s\n", prop.type.c_str());
            printf("\tDefault: %s\n", prop.def.c_str());
            printf("\n");
        }
    }

    std::filesystem::path opath(path);
    opath = opath / std::filesystem::path("generated.cpp");
    ref.writeGeneratedData(opath, classes);
}
