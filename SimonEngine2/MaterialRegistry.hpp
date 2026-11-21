#ifndef MATERIAL_REGISTRY_HPP
#define MATERIAL_REGISTRY_HPP

#include <string>
#include <unordered_map>
#include <iostream>

#include "EngineRender.hpp"

class MaterialRegistry {
public:
    static MaterialRegistry& getInstance() {
        static MaterialRegistry instance;
        return instance;
    }

    const EngineRender::Material* getOrCreate(const std::string& key, const EngineRender::Material& material);
    const EngineRender::Material* get(const std::string& key) const;
    void clear();

    MaterialRegistry(const MaterialRegistry&) = delete;
    MaterialRegistry& operator=(const MaterialRegistry&) = delete;
    MaterialRegistry(const MaterialRegistry&&) = delete;
    MaterialRegistry& operator=(const MaterialRegistry&&) = delete;

private:
    MaterialRegistry() = default;

    std::unordered_map<std::string, EngineRender::Material> materials_;
};

#endif