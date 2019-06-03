// Singleton class that provides access to assets across the project

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <string>
#include <unordered_map>

#include <RSM.h>

/** /
#include "Mesh.h"
#include "Model.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/Material.h"
/**/

// Macro to syntax sugar the singleton getter
// ex: RM.getTexture("albedo");

#define RM Resources::get()

namespace rsm {

	class Mesh;
	class Model;
	class Shader;
	class Texture;
	class Material;

	template<class KT, class T>
	using map = std::unordered_map<KT, T>;

	class Resources {
	public:
		~Resources();

		static Resources& get();

		void init();
		void cleanup();

		void addMesh(const std::string& name, const sref<Mesh>& mesh);
		void addModel(const std::string& name, const sref<Model>& model);
		void addShader(const std::string& name, const sref<Shader>& shader);
		void addTexture(const std::string& name, const sref<Texture>& texture);
		void addMaterial(const std::string& name, const sref<Material>& material);

		bool removeMesh(const std::string& name);
		bool removeModel(const std::string& name);
		bool removeShader(const std::string& name);
		bool removeTexture(const std::string& name);
		bool removeMaterial(const std::string& name);

		sref<Mesh> getMesh(const std::string& name);
		sref<Model> getModel(const std::string& name);
		sref<Shader> getShader(const std::string& name);
		sref<Texture> getTexture(const std::string& texture);
		sref<Material> getMaterial(const std::string& material);

	private:
		Resources();

		map<std::string, sref<Mesh>> _meshes;
		map<std::string, sref<Model>> _models;
		map<std::string, sref<Shader>> _shaders;
		map<std::string, sref<Texture>> _textures;
		map<std::string, sref<Material>> _materials;
	};
}

#endif // !__RESOURCES_H__
