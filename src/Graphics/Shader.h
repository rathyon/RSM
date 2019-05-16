#ifndef __RSM_SHADER_H__
#define __RSM_SHADER_H__

#include "Core/OpenGL.h"

namespace rsm {

	class ShaderSource {
	public:
		ShaderSource(ShaderType type, const std::string& filepath);
		ShaderSource(ShaderType type, std::string name, const char* source);
		~ShaderSource();

		GLuint id() const;
		ShaderType type() const;

		const std::string name() const;
		const std::string source() const;

		void compile();
		void inject(std::string str);

	private:
		GLuint _id;
		ShaderType _type;
		std::string _source;
		std::string _name;
	};

	class Shader {
	public:
		Shader(const std::string& name);

		GLuint id() const;
		const std::string& name() const;
		const std::vector<GLuint>& shaders() const;

		void addShader(const ShaderSource& source);
		bool link();

	private:
		GLuint _id;
		std::string _name;
		std::vector<GLuint> _shaders;
	};

}

#endif