#include "Scene.hpp"

#include <stdexcept>
#include <utility>

#include "Renderer.hpp"

#include "MaterialRegistry.hpp"
#include "MeshRegistry.hpp"
#include "TextureRegistry.hpp"

void Scene::addRootObject(std::unique_ptr<Object> object) {
	if (!object) {
		throw std::invalid_argument("Scene::addRootObject: object cannot be null");
	}

	if (object->hasParent()) {
		throw std::runtime_error("Scene::addRootObject: root object cannot have a parent");
	}

	if (isCurrentlyUpdating_) {
		pendingRootObjects_.push_back(std::move(object));
		return;
	}

	adoptRootObject(std::move(object));
}

void Scene::sceneInit() {
	init();
}

void Scene::sceneDeInit() {
	for (auto& rootObject : rootObjects_) {
		rootObject->rootOnDestruction();
	}

	rootObjects_.clear();
	pendingRootObjects_.clear();

	deInit();

	Renderer::getInstance().clear();
	MaterialRegistry::getInstance().clear();
	MeshRegistry::getInstance().clear();
	TextureRegistry::getInstance().clearRegistry();
}

void Scene::sceneSubmit() const {
	for (Light* light : lightObjects_) {
		if (light->isAlive() && light->isActive()) {
			light->submitLightInfo();
		}
	}

	for (CubeMap* cubeMap : cubeMapObjects_) {
		if (cubeMap->isAlive() && cubeMap->isActive()) {
			cubeMap->submitCubeMapInfo();
		}
	}

	draw();

	for (auto& rootObject : rootObjects_) {
		rootObject->rootDraw();
	}

	if (skybox_) {
		if (skybox_->isAlive() && skybox_->isActive()) {
			skybox_->submitSkyboxInfo();
		}
	}
}

void Scene::sceneUpdate() {
	update();

	isCurrentlyUpdating_ = true;

	for (auto& rootObject : rootObjects_) {
		rootObject->rootUpdate();
	}

	isCurrentlyUpdating_ = false;

	for (auto it = rootObjects_.begin(); it != rootObjects_.end();) {
		Object* rootObject = it->get();

		if (!rootObject->isAlive()) {
			rootObject->rootOnDestruction();
			it = rootObjects_.erase(it);
		}
		else {
			++it;
		}
	}

	processPendingRootObjects();
}

void Scene::adoptRootObject(std::unique_ptr<Object> object) {
	rootObjects_.push_back(std::move(object));

	Object* addedObject = rootObjects_.back().get();

	addedObject->rootOnStart();
	addedObject->invalidateWorldTransform();
	addedObject->updateWorldTransform();
}

void Scene::processPendingRootObjects() {
	for (auto& object : pendingRootObjects_) {
		adoptRootObject(std::move(object));
	}

	pendingRootObjects_.clear();
}

void Scene::addLight(Light* light) {
	lightObjects_.push_back(light);
}

void Scene::removeLight(Light* light) {
	for (auto it = lightObjects_.begin(); it != lightObjects_.end(); ++it) {
		if (light == *it) {
			lightObjects_.erase(it);
			return;
		}
	}
}

void Scene::addCubeMap(CubeMap* cubeMap) {
	cubeMapObjects_.push_back(cubeMap);
}

void Scene::removeCubeMap(CubeMap* cubeMap) {
	for (auto it = cubeMapObjects_.begin(); it != cubeMapObjects_.end(); ++it) {
		if (*it == cubeMap) {
			cubeMapObjects_.erase(it);
			return;
		}
	}
}

void Scene::addSkybox(Skybox* skybox) {
	if (!skybox) {
		throw std::invalid_argument("Scene::addSkybox: skybox cannot be null");
	}
	if (skybox_ && skybox_ != skybox) {
		throw std::runtime_error("Scene::addSkybox: scene already has a skybox");
	}

	skybox_ = skybox;
}

void Scene::removeSkybox(Skybox* skybox) {
	if (skybox_ == skybox) {
		skybox_ = nullptr;
	}
}