#include "Shader.h"

#include <iostream>
#include <fstream>

#include "Utils/Utils.h"

namespace rsm {

	ShaderSource::ShaderSource(ShaderType type, const std::string& filepath) {
		_name = filepath;
		_id = glCreateShader(OpenGLShaderType[type]);
		if (_id == 0) {
			//std::cerr << "Could not create shader: " + _name << std::endl;
			LOGE("Could not create shader: " + _name);
		}
			
		// Read shader source code into attribute
		Utils::readFile(filepath, std::ios_base::in, _source);

	}

	ShaderSource::~ShaderSource() {
		glDeleteShader(_id);
	}

	GLuint ShaderSource::id() const {
		return _id;
	}

	ShaderType ShaderSource::type() const {
		return _type;
	}

	const std::string ShaderSource::name() const {
		return _name;
	}

	const std::string ShaderSource::source() const {
		return _source;
	}

	void ShaderSource::compile() {
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

			//std::cerr << "Couldn't compile shader: " << _name << "\n Compilation log:\n" << message;
			LOGE("Couldn't compile shader: " + _name + "\n Compilation log:\n" + message);
		}
	}

	// Injects a string on the first line of the shader source
	void ShaderSource::inject(const std::string& str) {
		_source.insert(0, str);
	}



	Shader::Shader(const std::string& name) {
		_name = name;
	}

	GLuint Shader::id() const {
		return _id;
	}

	const std::string& Shader::name() const{
		return _name;
	}

	const std::vector<ShaderSource>& Shader::shaders() const {
		return _shaders;
	}

	void Shader::addShader(const ShaderSource& src) {
		_shaders.push_back(src);
	}

	bool Shader::link() {
		_id = glCreateProgram();
		if (_id == 0) {
			//std::cerr << "Could not create shader: " + _name << std::endl;
			LOGE("Could not create shader: " + _name);
		}
			
		for (ShaderSource src : _shaders) {
			glAttachShader(_id, src.id());
			// Check attachment error
			checkOpenGLError("Could not attach shader (" + std::to_string(src.id()) +
				") to program (" + std::to_string(_id) + ") " + src.name() + ".");
		}

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

			//std::cerr << "Shader " << _name << " linkage log:\n" << strLog;
			LOGE("Shader " + _name + " linkage log:\n" + strLog);

			delete[] log;

			// Detach shaders
			for (ShaderSource src : _shaders) {
				glDetachShader(_id, src.id());
			}

			// Delete the program
			glDeleteProgram(_id);

			return false;
		}

		// Detach shaders after successful linking
		for (ShaderSource src : _shaders) {
			glDetachShader(_id, src.id());
		}

		return true;
	}
}