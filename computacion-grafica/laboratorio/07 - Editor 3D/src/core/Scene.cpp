#include "core/Scene.hpp"

#include <algorithm>

SceneObject& Scene::addObject(ObjectType type) {
    SceneObject object;
    object.id = this->nextId_++;
    object.type = type;
    object.position = {static_cast<float>(this->objects_.size()) * 1.5f, 0.0f, 0.0f};
    object.color = {
        0.35f + 0.15f * static_cast<float>(object.id % 3),
        0.45f + 0.10f * static_cast<float>((object.id + 1) % 3),
        0.70f + 0.08f * static_cast<float>((object.id + 2) % 3)
    };
    this->objects_.push_back(object);
    this->selectedId_ = object.id;
    return this->objects_.back();
}

SceneObject* Scene::selectedObject() {
    return this->findById(this->selectedId_);
}

const SceneObject* Scene::selectedObject() const {
    auto it = std::find_if(this->objects_.begin(), this->objects_.end(), [this](const SceneObject& object) {
        return object.id == this->selectedId_;
    });
    return it == this->objects_.end() ? nullptr : &(*it);
}

SceneObject* Scene::findById(int id) {
    auto it = std::find_if(this->objects_.begin(), this->objects_.end(), [id](const SceneObject& object) {
        return object.id == id;
    });
    return it == this->objects_.end() ? nullptr : &(*it);
}

const std::vector<SceneObject>& Scene::objects() const {
    return this->objects_;
}

std::vector<SceneObject>& Scene::objects() {
    return this->objects_;
}

void Scene::selectNext() {
    if (this->objects_.empty()) {
        this->selectedId_ = -1;
        return;
    }
    auto it = std::find_if(this->objects_.begin(), this->objects_.end(), [this](const SceneObject& object) {
        return object.id == this->selectedId_;
    });
    if (it == this->objects_.end() || ++it == this->objects_.end()) {
        this->selectedId_ = this->objects_.front().id;
    } else {
        this->selectedId_ = it->id;
    }
}

void Scene::selectPrevious() {
    if (this->objects_.empty()) {
        this->selectedId_ = -1;
        return;
    }
    auto it = std::find_if(this->objects_.begin(), this->objects_.end(), [this](const SceneObject& object) {
        return object.id == this->selectedId_;
    });
    if (it == this->objects_.begin() || it == this->objects_.end()) {
        this->selectedId_ = this->objects_.back().id;
    } else {
        this->selectedId_ = (--it)->id;
    }
}

void Scene::selectById(int id) {
    this->selectedId_ = this->findById(id) ? id : -1;
}

void Scene::duplicateSelected() {
    SceneObject* selected = this->selectedObject();
    if (!selected) {
        return;
    }
    SceneObject copy = *selected;
    copy.id = this->nextId_++;
    copy.position = copy.position + Vec3{0.8f, 0.0f, 0.8f};
    this->objects_.push_back(copy);
    this->selectedId_ = copy.id;
}

void Scene::deleteSelected() {
    if (this->selectedId_ < 0) {
        return;
    }
    this->objects_.erase(std::remove_if(this->objects_.begin(), this->objects_.end(), [this](const SceneObject& object) {
        return object.id == this->selectedId_;
    }), this->objects_.end());
    this->selectedId_ = this->objects_.empty() ? -1 : this->objects_.front().id;
}

void Scene::clear() {
    this->objects_.clear();
    this->selectedId_ = -1;
    this->nextId_ = 1;
}

int Scene::selectedId() const {
    return this->selectedId_;
}

int Scene::nextId() const {
    return this->nextId_;
}

void Scene::setNextId(int nextIdValue) {
    this->nextId_ = std::max(1, nextIdValue);
}
