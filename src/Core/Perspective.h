#ifndef __RSM_PERSPECTIVE_H__
#define __RSM_PERSPECTIVE_H__

#include "Camera.h"

namespace rsm {

	class Perspective : public Camera {
	public:
		Perspective();
		Perspective(int width, int height, glm::vec3 eye, glm::vec3 at, glm::vec3 up, float n, float f, float fov);

		float fov() const;
		void updateProjMatrix(int width, int height) override;

	private:
		float _fov;
	};

}

#endif // !__RSM_PERSPECTIVE_H__