#ifndef __RSM_SCENE_H__
#define __RSM_SCENE_H__

#include <RSM.h>

#include "Camera.h"
#include "Lights\Light.h"
#include "Model.h"

namespace rsm {

	class Scene {
	public:
		Scene();

		void addCamera(const sref<Camera>& camera);
		void addLight(const sref<Light>& light);
		void addModel(const sref<Model>& model);

		const std::vector<sref<Camera>>& cameras() const;
		const std::vector<sref<Light>>& lights() const;
		const std::vector<sref<Model>>& models() const;

		void draw();

	private:
		std::vector<sref<Camera>> _cameras;
		std::vector<sref<Light>> _lights;
		std::vector<sref<Model>> _models;
	};

}

#endif