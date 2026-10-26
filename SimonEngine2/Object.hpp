#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <vector>
#include <memory>
#include <stdexcept>
#include <string>

#include "EngineMath.hpp"

class Object {
	public:
		Object() = default;
		virtual ~Object() = default;

		// Users really shouldnt override rootOnStart/Destruction unless they know what they are doing
		void rootOnStart(); // Runs when attached to scene
		void rootUpdate(); // Runs once per update of the Scene
		void rootDraw(); // Runs once per frame update of the Scene
		void rootOnDestruction(); // Runs on automatically on Scene destruction and next update after destroy

		bool isAlive() const { return isAlive_; }
		void destroy() { isAlive_ = false; } // Call this function or just change isAlive to delete GameObject

		bool hasParent() const { return parent_ != nullptr; }
		Object* getParent() const { return parent_; }
		bool isWorldTransformOutDated() const { return isWorldTransformOutDated_; }

		const EngineMath::Transform& getLocalTransform() const { return localTransform_; }
		const EngineMath::Transform& getWorldTransform() const { return worldTransform_; }

		void setPosition(EngineMath::Vector3 pos) { localTransform_.position_ = pos; onLocalTransformChanged(); }
		void setScale(EngineMath::Vector3 scale) { localTransform_.scale_ = scale; onLocalTransformChanged(); }
		void setRotation(EngineMath::Quaternion rotation) { localTransform_.rotation_ = rotation.normalized(); onLocalTransformChanged(); }
		void setLocalTransform(EngineMath::Transform transform) { localTransform_ = transform; onLocalTransformChanged(); }

		void invalidateWorldTransform();
		void updateWorldTransform();

		void addChild(std::unique_ptr<Object> child); // Attaches a child to a GameObject
		std::unique_ptr<Object> orphanChild(Object* child);
		void reparentChild(Object* child, Object* newParent);

		const std::vector<std::unique_ptr<Object>>& getChildren() const { return children_; }

		std::string getTag() const { return tag_; }
		std::string getName() const { return name_; }

		bool isDrawing() const { return isDrawing_; }
		bool isUpdating() const { return isUpdating_; }
		void toggleDrawing() { isDrawing_ = !isDrawing_; }
		void toggleUpdating() { isUpdating_ = !isUpdating_; }

	protected:
		virtual void onStart() {} // Runs when attached to scene
		virtual void update() {} // Runs once per update of the Scene
		virtual void draw() const {} // Runs once per frame update of the Scene
		virtual void onDestruction() {} // Runs on automatically on Scene destruction and next update after destroy

		Object* parent_ = nullptr;

		std::string tag_ = "N/A";
		std::string name_ = "Object";
		EngineMath::Transform localTransform_;

		void onLocalTransformChanged() { invalidateWorldTransform(); }

		const EngineMath::Matrix4& getWorldMatrix() const { return worldMatrix_; }

		std::vector<std::unique_ptr<Object>> children_ = {};

		bool hasStarted_ = false;
		bool isDrawing_ = true;
		bool isUpdating_ = true;

	private:
		std::unique_ptr<Object> extractChild(Object* child);

		bool isWorldTransformOutDated_ = true;
		EngineMath::Transform worldTransform_;
		EngineMath::Matrix4 worldMatrix_ = EngineMath::Matrix4::identity();

		bool isAlive_ = true;
};

#endif