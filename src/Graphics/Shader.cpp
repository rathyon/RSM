#include "Shader.h"

#include <iostream>
#include <fstream>
#include "../Utils/Utils.h"

namespace rsm {

	Shader::Shader(ShaderType type, const std::string& filepath) {
		_name = filepath;
		_id = glCreateShader(OpenGLShaderType[type]);
		if (_id == 0)
			std::cerr << "Could not create shader: " + _name << std::endl;

		// Read shader source code into attribute
		Utils::readFile(filepath, std::ios_base::in, _source);
	}

	Shader::Shader(const std::string& name) {
		_name = name;
		_id = glCreateProgram();
	}

	void Shader::compile() {
		const char* c_str = _source.c_str();
		glShaderSource(_id, 1, &c_str, 0);
		glCompileShader(_id);

		// Check if shader compiled
		GLint result;
		glGetShaderiv(_id, GL_COMPILE_STATUS, &result);

		if (result != GL_TRUE) {
			// Check shader log for the error and print it
			GLint logLen;
			glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &logLen);

			char* log = new char[logLen];
			glGetShaderInfoLog(_id, logLen, &logLen, log);

			std::string message(log);
			delete[] log;

			std::cerr << "Couldn't compile shader: " << _name << "\n Compilation log:\n" << message;
		}
	}

	// Injects a string on the first line of the shader source
	void Shader::inject(const std::string& str) {
		_source.insert(0, str);
	}

	void Shader::attach(Shader shader) {
		glAttachShader(_id, shader.id());
		// Check attachment error
		checkOpenGLError("Could not attach shader (" + std::to_string(shader.id()) +
			") to program (" + std::to_string(_id) + ") " + shader.name() + ".");

		_shaders.push_back(shader.id());
	}

	void Shader::link() {
		glLinkProgram(_id);

		// Check linkage for errors and print them
		GLint res;
		glGetProgramiv(_id, GL_LINK_STATUS, &res);
		if (res != GL_TRUE) {
			GLint logLen;
			glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLen);

			char* log = new char[logLen];
			glGetProgramInfoLog(_id, logLen, &logLen, log);

			std::string strLog(log);
			std::cerr << "Shader " << _name << " linkage log:\n" << strLog;
			delete[] log;
		}

		for (int i = 0; i < _shaders.size(); i++) {
			glDeleteShader(_shaders[i]);
		}
	}

	Shader::~Shader() {
		glDeleteShader(_id);
	}

	GLuint Shader::id(){
		return _id;
	}

	ShaderType Shader::type() {
		return _type;
	}

	std::string Shader::name() {
		return _name;
	}

	std::string Shader::source() {
		return _source;
	}

}