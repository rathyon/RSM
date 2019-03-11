#ifndef __RSM_SHADER_H__
#define __RSM_SHADER_H__

#include "Core/OpenGL.h"

namespace rsm {

	const GLenum OpenGLShaderType[] {
		GL_VERTEX_SHADER,
		GL_FRAGMENT_SHADER,
		//GL_GEOMETRY_SHADER, // OpenGL ES doesn't have it
		//GL_COMPUTE_SHADER	// OpenGL ES doesn't have it
	};

	const enum ShaderType {
		VERTEX_SHADER     = 0,
		FRAGMENT_SHADER   = 1,
		//GEOMETRY_SHADER   = 2,
		//COMPUTE_SHADER    = 3,
		//SHADER_TYPE_COUNT = 4
	};

	class ShaderSource {
	public:
		ShaderSource(ShaderType type, const std::string& filepath);
		~ShaderSource();

		GLuint id() const;
		ShaderType type() const;

		const std::string name() const;
		const std::string source() const;

		void compile();
		void inject(const std::string& str);

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
		const std::vector<ShaderSource>& shaders() const;

		void addShader(const ShaderSource& source);
		bool link();

	private:
		GLuint _id;
		std::string _name;
		std::vector<ShaderSource> _shaders;
	};

}

#endif