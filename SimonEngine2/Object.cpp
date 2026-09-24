#include "Object.hpp"

// Runs when attached to scene
void Object::rootOnStart() {
	if (hasStarted_) {
		return;
	}
	hasStarted_ = true;

	onStart();

	isWorldTransformOutDated_ = true;

	for (auto& child : children_) {
		child->rootOnStart();
	}
}

// Runs once per update of the Scene
void Object::rootUpdate() {
	if (isWorldTransformOutDated_ ||
		(parent_ && parent_->isWorldTransformOutDated_)) {
		updateWorldTransform();
	}

	update();

	if (isWorldTransformOutDated_) {
		updateWorldTransform();
	}

	for (auto it = children_.begin(); it != children_.end();) {
		Object* child = it->get();

		child->rootUpdate();

		if (child->isAlive()) {
			++it;
		}
		else {
			child->rootOnDestruction();
			it = children_.erase(it);
		}
	}
}

// Runs once per frame update of the Scene
void Object::rootDraw() {
	if (!isDrawing_) {
		return;
	}

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	EngineMath::Matrix4 localMatrix = localTransform_.toMatrix();
	glMultMatrixf(localMatrix.data());

	draw();

	for (auto& child : children_) {
		child->rootDraw();
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

// Runs after right before being unattached from scene
void Object::rootOnDestruction() {
	onDestruction();

	for (auto& child : children_) {
		child->rootOnDestruction();
	}
}

void Object::invalidateWorldTransform() {
	isWorldTransformOutDated_ = true;

	for (auto& child : children_) {
		child->invalidateWorldTransform();
	}
}

void Object::updateWorldTransform() {
	if (parent_) {
		worldTransform_ =
			localTransform_.composedWith(parent_->worldTransform_);
	}
	else {
		worldTransform_ = localTransform_;
	}

	isWorldTransformOutDated_ = false;

	for (auto& child : children_) {
		child->updateWorldTransform();
	}
}

// Attaches another Object (child) to a Object
void Object::addChild(std::unique_ptr<Object> child) {
	if (!child) {
		throw std::invalid_argument("Object::attachChild: object cannot be null");
	}
	if (child.get() == this) {
		throw std::invalid_argument("Object::attachChild: object cannot parent itself");
	}
	if (child->parent_) {
		throw std::runtime_error("Object::attachChild : child already has a parent");
	}

	for (Object* ancestor = this; ancestor != nullptr; ancestor = ancestor->parent_) {
		if (ancestor == child.get()) {
			throw std::runtime_error("Object::attachChild: operation would create a hierarchy cycle");
		}
	}

	Object* attachedChild = child.get();

	children_.push_back(std::move(child));

	attachedChild->parent_ = this;
	attachedChild->invalidateWorldTransform();

	if (hasStarted_) {
		attachedChild->rootOnStart();
	}
}

// Removes a child from any hierarchy (If you want it attached to scene as root do so yourself)
// Just a public exposure of extractChild
std::unique_ptr<Object> Object::orphanChild(Object* child) {
	if (child->parent_ != this) {
		throw std::runtime_error("Object::orphanChild: object is not a direct child of this object");
	}
	return extractChild(child);
}

// Gives a specific Child to another Object
void Object::reparentChild(Object* child, Object* newParent) {
	if (!child) {
		throw std::invalid_argument("Object::reparentChild: child cannot be null");
	}
	if (!newParent) {
		throw std::invalid_argument("Object::reparentChild: new parent cannot be null");
	}
	if (child->parent_ != this) {
		throw std::runtime_error("Object::reparentChild: object is not a child of this object");
	}
	if (newParent == this) {
		return;
	}

	for (Object* ancestor = this; ancestor != nullptr; ancestor = ancestor->parent_) {
		if (ancestor == child) {
			throw std::runtime_error("Object::attachChild: operation would create a hierarchy cycle");
		}
	}

	std::unique_ptr<Object> transferred = extractChild(child);
	newParent->addChild(std::move(transferred));
}

// Removes child Object from this Object, returns removed Object
std::unique_ptr<Object> Object::extractChild(Object* child) {
	if (!child) {
		throw std::invalid_argument("Object::extractChild: child cannot be null");
	}
	if (child->parent_ != this) {
		throw std::runtime_error("Object::extractChild: object is not a child of this object");
	}

	for (auto it = children_.begin(); it != children_.end(); ++it) {
		if (it->get() != child) {
			continue;
		}

		std::unique_ptr<Object> extracted = std::move(*it);
		children_.erase(it);

		extracted->parent_ = nullptr;
		extracted->invalidateWorldTransform();

		return extracted;
	}

	throw std::logic_error("Object::extractChild: hierarchy is internally inconsistent");
}