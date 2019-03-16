// Singleton class that provides access to assets across the project

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <string>
#include <unordered_map>

#include <RSM.h>

// Macro to syntax sugar the singleton getter
// ex: RM.getTexture("albedo");

#define RM Resources::get()

namespace rsm {

	class Mesh;
	class Model;
	class Shader;

	template<class KT, class T>
	using map = std::unordered_map<KT, T>;

	class Resources {
	public:
		~Resources();

		static Resources& get();

		void init();
		void cleanup();

		void addMesh(const std::string& name, const std::shared_ptr<Mesh>& mesh);
		void addModel(const std::string& name, const std::shared_ptr<Model>& model);
		void addShader(const std::string& name, const std::shared_ptr<Shader>& shader);

		bool removeMesh(const std::string& name);
		bool removeModel(const std::string& name);
		bool removeShader(const std::string& name);

		Mesh* getMesh(const std::string& name);
		Model* getModel(const std::string& name);
		Shader* getShader(const std::string& name);

	private:
		Resources();

		map<std::string, std::shared_ptr<Mesh>> _meshes;
		map<std::string, std::shared_ptr<Model>> _models;
		map<std::string, std::shared_ptr<Shader>> _shaders;
	};
}

#endif // !__RESOURCES_H__
