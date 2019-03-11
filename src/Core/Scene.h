#ifndef __RSM_SCENE_H__
#define __RSM_SCENE_H__

#include <RSM.h>

namespace rsm {

	class Model;

	template<class T>
	using vec = std::vector<T>;

	class Scene {
	public:
		Scene();

		void addModel(const sref<Model>& model);

		const vec<sref<Model>>& models() const;

	private:
		vec<sref<Model>> _models;
	};

}

#endif