#ifndef __RSM_SCENEOBJ_H__
#define __RSM_SCENEOBJ_H__

#include <RSM.h>
#include <RSMaths.h>

using namespace glm;

namespace rsm {

	class SceneObject {
	public:
		SceneObject();
		SceneObject(const vec3& position);

		vec3& position();
		vec3& scale();
		quat& rotation();

		sref<SceneObject> parent() const;

		void setPosition(const vec3& position);
		void setScale(const float x, const float y, const float z);
		void setRotation(const quat& rotation);

		//virtual void updateMatrix();

	protected:
		vec3 _position;
		vec3 _scale;
		quat _rotation;

		sref<SceneObject> _parent;
	};

}

#endif