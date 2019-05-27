#ifndef __RSM_MODEL_H__
#define __RSM_MODEL_H__

#include "SceneObject.h"
#include "Mesh.h"
#include "Graphics\BlinnPhongMaterial.h"
#include "Graphics\Texture.h"

namespace rsm {

	class Model : public SceneObject {
	public:
		Model(const std::string& name);
		Model(const std::string& name, const vec3& position);
		Model(const std::string& name, const mat4& objToWorld);

		~Model();

		void loadFromFile(const std::string& filepath, const std::string& matpath);
		void loadFromMemory(const char* objsource, const char* matsource);

		// set user defined material to all meshes and submeshes
		void setMaterial(sref<Material> material);

		std::vector<sref<Mesh>>& meshes();
		const mat3& normalMatrix() const;

		void updateMatrix() override;

		void prepare();
		void render();
		void draw(sref<Shader> program);

	private:
		std::string _name;
		std::vector<sref<Mesh>> _meshes;
		mat3 _normalMatrix;

		bool loadObj(bool fromFile, const char* objsource, const char* matsource);
	};

}

#endif