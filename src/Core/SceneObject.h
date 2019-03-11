#ifndef __RSM_SCENEOBJ_H__
#define __RSM_SCENEOBJ_H__

#include <RSM.h>
#include <RSMaths.h>

namespace rsm {

	class SceneObject {
	public:
		SceneObject();
		SceneObject(const glm::vec3& position);

		glm::vec3& position();
		glm::vec3& scale();
		glm::quat& rotation();

		sref<SceneObject> parent() const;

		void setPosition(const glm::vec3& position);
		void setScale(const float x, const float y, const float z);
		void setRotation(const glm::quat& rotation);

		//virtual void updateMatrix();

	protected:
		glm::vec3 _position;
		glm::vec3 _scale;
		glm::quat _rotation;

		sref<SceneObject> _parent;
	};

}

#endif