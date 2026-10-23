#include "Scene.hpp"
#include "ChuckEdition.hpp"

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

	MeshRegistry::getInstance().clearRegistry();
}

void Scene::sceneDraw() const {
	camera_.apply();

	// Studio lights + anything else that has to live in world space with the camera applied
	ChuckEdition::getInstance().beginWorld(*this);

	draw();

	drawRootObjects();

	// Neon edge glow: the same objects again, drawn as glowing outlines.
	// (0 passes when F6 turned the outlines off)
	ChuckEdition& chuck = ChuckEdition::getInstance();
	for (int pass = 0; pass < chuck.outlinePassCount(); ++pass) {
		chuck.beginOutlinePass(pass);
		drawRootObjects();
		chuck.endOutlinePass();
	}

	// Neon floor under everything, then all world state gets rolled back
	chuck.endWorld(*this);
}

namespace {
	std::size_t countHierarchy(Object& object) {
		std::size_t count = 1;
		for (auto& child : object.getChildren()) {
			count += countHierarchy(*child);
		}
		return count;
	}
}

std::size_t Scene::countObjects() const {
	std::size_t count = 0;
	for (auto& rootObject : rootObjects_) {
		count += countHierarchy(*rootObject);
	}
	return count;
}

void Scene::drawRootObjects() const {
	for (auto& rootObject : rootObjects_) {
		rootObject->rootDraw();
	}
}

void Scene::sceneUpdate() {
	update();

	isCurrentlyUpdating_ = true;

	for (auto& rootObject : rootObjects_) {
		rootObject->rootUpdate();
	}

	isCurrentlyUpdating_ = false;

	for (auto it = rootObjects_.begin();
		it != rootObjects_.end();) {
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
