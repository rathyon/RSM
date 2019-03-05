#ifndef __RSM_SHADER_H__
#define __RSM_SHADER_H__

#include "Core/OpenGL.h"
#include <string>
#include <vector>

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

	class Shader {
	public:
		// Constructor for shader sources
		Shader(ShaderType type, const std::string& filepath);

		// Constructor for shader programs
		Shader(const std::string& name);
		~Shader();

		GLuint id();
		ShaderType type();
		std::string name();
		std::string source();

		void inject(const std::string& str);

		void compile();
		void attach(Shader shader);
		void link();

	private:
		GLuint _id;
		ShaderType _type;
		std::string _source;
		std::string _name;
		std::vector<GLuint> _shaders;
	};

}

#endif