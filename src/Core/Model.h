#ifndef __RSM_MODEL_H__
#define __RSM_MODEL_H__

#include "SceneObject.h"
#include "Mesh.h"
#include "Graphics\Material.h"

namespace rsm {

	class Model : public SceneObject {
	public:
		Model(const std::string& name);
		Model(const std::string& name, const vec3& position);
		Model(const std::string& name, const mat4& objToWorld);

		~Model();

		void loadFromFile(const std::string& filepath);
		void loadFromMemory(const char* source);

		// temporary solution
		void setMaterial(sref<Material> material);

		std::vector<sref<Mesh>>& meshes();
		const mat3& normalMatrix() const;

		void updateMatrix() override;

		void prepare();
		void draw();

	private:
		std::string _name;
		std::vector<sref<Mesh>> _meshes;
		mat3 _normalMatrix;

		bool loadObj(bool fromFile, const char* source);
	};

}

#endif