#include "MaterialRegistry.hpp"

#include <utility>

const EngineRender::Material* MaterialRegistry::getOrCreate(const std::string& key, const EngineRender::Material& material) {
    const auto found = materials_.find(key);
    if (found != materials_.end()) {
        return &found->second;
    }

    EngineRender::Material initialized = material;
    initialized.init();

    const auto inserted = materials_.emplace(key, std::move(initialized));
    return &inserted.first->second;
}

const EngineRender::Material* MaterialRegistry::get(const std::string& key) const {
    const auto found = materials_.find(key);
    if (found != materials_.end()) {
        return &found->second;
    }

    std::cout << "MaterialRegistry::get: Failed to get " << key << std::endl;

    return nullptr;
}

void MaterialRegistry::clear() {
    materials_.clear();
}