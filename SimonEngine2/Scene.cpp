#include "Scene.hpp"

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
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	bool projOrtho = false;

	if (projOrtho) {
		glOrtho(-1.0, 1.0, -1.0, 1.0, -1.0, 1.0);
	}
	else {
		double aspectRatio = static_cast<double>(ENGINE_WIN_W) / static_cast<double>(ENGINE_WIN_H);

		gluPerspective(60.0, aspectRatio, 0.1, 100.0);
	}


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	init();
}

void Scene::sceneDeInit() {
	for (auto& rootObject : rootObjects_) {
		rootObject->rootOnDestruction();
	}

	rootObjects_.clear();
	pendingRootObjects_.clear();

	deInit();
}

void Scene::sceneDraw() const {
	draw();
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