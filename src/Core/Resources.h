// Singleton class that provides access to assets across the project

#ifndef __RESOURCES_H__
#define __RESOURCES_H__

#include <string>
#include <unordered_map>

#include "../RSM.h"

// Macro to syntax sugar the singleton getter
// ex: RM.getTexture("albedo");

#define RM Resources::get()

namespace rsm {

	class Shader;

	template<class KT, class T>
	using map = std::unordered_map<KT, T>;

	class Resources {
	public:
		~Resources();

		static Resources& get();

		void init();

		void addShader(const std::string& name, const std::shared_ptr<Shader>& shader);

		bool removeShader(const std::string& name);

		Shader* getShader(const std::string& name);

		void cleanup();

	private:
		Resources();

		map<std::string, std::shared_ptr<Shader>> _shaders;
	};
}

#endif // !__RESOURCES_H__
