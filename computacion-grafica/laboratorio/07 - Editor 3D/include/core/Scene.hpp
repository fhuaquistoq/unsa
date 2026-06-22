#pragma once

#include <cstddef>
#include <vector>

#include "core/SceneObject.hpp"

class Scene {
public:
    SceneObject& addObject(ObjectType type);
    SceneObject* selectedObject();
    const SceneObject* selectedObject() const;
    SceneObject* findById(int id);
    const std::vector<SceneObject>& objects() const;
    std::vector<SceneObject>& objects();

    void selectNext();
    void selectPrevious();
    void selectById(int id);
    void duplicateSelected();
    void deleteSelected();
    void clear();

    int selectedId() const;
    int nextId() const;
    void setNextId(int nextIdValue);

private:
    std::vector<SceneObject> objects_;
    int selectedId_ = -1;
    int nextId_ = 1;
};
