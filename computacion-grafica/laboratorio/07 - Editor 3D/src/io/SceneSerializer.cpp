#include "io/SceneSerializer.hpp"

#include <fstream>
#include <sstream>

bool SceneSerializer::load(const std::string& path, Scene& scene, EditorState& state) {
    std::ifstream input(path);
    if (!input) {
        return false;
    }

    scene.clear();
    std::string line;
    int selectedId = -1;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream stream(line);
        std::string tag;
        stream >> tag;
        if (tag == "next_id") {
            int nextId = 1;
            stream >> nextId;
            scene.setNextId(nextId);
        } else if (tag == "selected") {
            stream >> selectedId;
        } else if (tag == "camera") {
            stream >> state.camera.yaw >> state.camera.pitch >> state.camera.distance
                   >> state.camera.target.x >> state.camera.target.y >> state.camera.target.z;
        } else if (tag == "projection") {
            stream >> state.projection.fov >> state.projection.nearPlane >> state.projection.farPlane;
        } else if (tag == "wireframe") {
            int value = 0;
            stream >> value;
            state.wireframe = value != 0;
        } else if (tag == "object") {
            SceneObject object;
            std::string typeName;
            stream >> object.id >> typeName
                   >> object.position.x >> object.position.y >> object.position.z
                   >> object.rotation.x >> object.rotation.y >> object.rotation.z
                   >> object.scale.x >> object.scale.y >> object.scale.z
                   >> object.color.x >> object.color.y >> object.color.z;
            if (objectTypeFromString(typeName, object.type)) {
                scene.objects().push_back(object);
                if (object.id >= scene.nextId()) {
                    scene.setNextId(object.id + 1);
                }
            }
        }
    }

    if (!scene.objects().empty()) {
        scene.selectById(selectedId);
        if (!scene.selectedObject()) {
            scene.selectNext();
        }
    }
    return true;
}

bool SceneSerializer::save(const std::string& path, const Scene& scene, const EditorState& state) {
    std::ofstream output(path);
    if (!output) {
        return false;
    }

    output << "# Editor3D scene v1\n";
    output << "next_id " << scene.nextId() << "\n";
    output << "selected " << scene.selectedId() << "\n";
    output << "camera " << state.camera.yaw << ' ' << state.camera.pitch << ' ' << state.camera.distance << ' '
           << state.camera.target.x << ' ' << state.camera.target.y << ' ' << state.camera.target.z << "\n";
    output << "projection " << state.projection.fov << ' ' << state.projection.nearPlane << ' '
           << state.projection.farPlane << "\n";
    output << "wireframe " << (state.wireframe ? 1 : 0) << "\n";

    for (const SceneObject& object : scene.objects()) {
        output << "object " << object.id << ' ' << objectTypeToString(object.type) << ' '
               << object.position.x << ' ' << object.position.y << ' ' << object.position.z << ' '
               << object.rotation.x << ' ' << object.rotation.y << ' ' << object.rotation.z << ' '
               << object.scale.x << ' ' << object.scale.y << ' ' << object.scale.z << ' '
               << object.color.x << ' ' << object.color.y << ' ' << object.color.z << "\n";
    }
    return true;
}
