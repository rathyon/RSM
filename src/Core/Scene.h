#ifndef __RSM_SCENE_H__
#define __RSM_SCENE_H__

#include <RSM.h>

namespace rsm {

	class Model;

	class Scene {
	public:
		Scene();

		void addModel(const sref<Model>& model);

		const std::vector<sref<Model>>& models() const;

	private:
		std::vector<sref<Model>> _models;
	};

}

#endif