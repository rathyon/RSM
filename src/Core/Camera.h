#ifndef __RSM_CAMERA_H__
#define __RSM_CAMERA_H__

#include "SceneObject.h"

namespace rsm {

	struct CameraData {
		glm::mat4 viewMatrix;
		glm::mat4 projMatrix;
		glm::mat4 viewProjMatrix;
		glm::vec3 viewPos;
	};

	class Camera : public SceneObject {
	public:
		Camera();
		Camera(int width, int height,
			const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up,
			float n, float f);

		int width() const;
		int height() const;

		float aspect() const;
		float n() const;
		float f() const;

		glm::vec3 right() const;
		glm::vec3 front() const;
		glm::vec3 up() const;

		void lookAt(const glm::vec3& eye, const glm::vec3& at, const glm::vec3& up);
		void lookAt(const glm::vec3& at);

		const glm::mat4& viewMatrix() const;
		const glm::mat4& projMatrix() const;

		glm::mat4 viewProjMatrix() const;

		void updateViewMatrix();
		virtual void updateProjMatrix(int width, int height);
		void updateOrientation(float dp, float dy);

		void toData(CameraData& data);

		void info();

	protected:
		glm::mat4 _projMatrix;

		int _width;
		int _height;

		float _near;
		float _far;

		// in radians
		float _pitch; // rotation over right vector
		float _yaw;   // rotation over up vector
	};

}

#endif // !__RSM_CAMERA_H__
