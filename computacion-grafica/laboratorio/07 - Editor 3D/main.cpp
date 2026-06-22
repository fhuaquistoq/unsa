#include <iostream>
#include <filesystem>
#include <string>

#include "app/EditorApp.hpp"

static std::string normalizeScenePath(const std::string& scenePath) {
    std::filesystem::path path(scenePath);

    if (path.extension() == ".f3d") {
        return path.string();
    }

    path.replace_extension(".f3d");
    return path.string();
}

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: editor nombre_archivo[.f3d]" << std::endl;
        return 1;
    }

    EditorApp app(argc, argv, normalizeScenePath(argv[1]));
    return app.run();
}
