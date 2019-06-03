#ifndef __RSM_SCENEOBJ_H__
#define __RSM_SCENEOBJ_H__

#include <RSM.h>
#include "Math/RSMaths.h"

namespace rsm {

	class SceneObject {
	public:
		SceneObject();
		SceneObject(const glm::vec3& position);
		SceneObject(const glm::mat4& objToWorld);

		glm::vec3& position();
		glm::vec3& scale();
		glm::quat& rotation();
		glm::mat4& objToWorld();

		sref<SceneObject> parent() const;

		void setPosition(const glm::vec3& position);
		void setScale(const float x, const float y, const float z);
		void setRotation(const glm::quat& rotation);
		void setObjToWorld(const glm::mat4& mat);

		virtual void updateMatrix();

	protected:
		glm::vec3 _position;
		glm::vec3 _scale;
		glm::quat _rotation;

		glm::mat4 _objToWorld;

		sref<SceneObject> _parent;
	};

}

#endif