#include "Shader.h"

#include <iostream>
#include <fstream>

#include "Utils/Utils.h"

namespace rsm {

	ShaderSource::ShaderSource(ShaderType type, const std::string& filepath) {
		_name = filepath;
		_id = glCreateShader(OpenGLShaderType[type]);
		if (_id == 0) {
			LOGE("Could not create shader:\n");
			LOGE("%s\n", _name.c_str());
		}

		// Read shader source code into attribute
		Utils::readFile(filepath, std::ios_base::in, _source);

	}

	ShaderSource::ShaderSource(ShaderType type, std::string name, const char* source) {
		_name = name;
		_id = glCreateShader(OpenGLShaderType[type]);
		if (_id == 0) {
			LOGE("Could not create shader:\n");
			LOGE("%s\n", _name.c_str());
		}

		_source = std::string(source);
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

			LOGE("Couldn't compile shader:\n");
			LOGE("%s\n", _name.c_str());
			LOGE("Compilation log:\n");
			LOGE("%s\n", message.c_str());

			//cleanup the shader
			glDeleteShader(_id);
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

	const std::vector<GLuint>& Shader::shaders() const {
		return _shaders;
	}

	void Shader::addShader(const ShaderSource& src) {
		_shaders.push_back(src.id());
	}

	bool Shader::link() {

		_id = glCreateProgram();
		if (_id == 0) {

			GLint logLen;
			glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logLen);

			char* log = new char[logLen];
			glGetProgramInfoLog(_id, logLen, &logLen, log);

			std::string strLog(log);
			delete[] log;

			LOGE("Could not create shader:\n");
			LOGE("%s\n", _name.c_str());
		}

		for (GLuint sid : _shaders) {
			glAttachShader(_id, sid);
			// Check attachment error
			checkOpenGLError("Could not attach shader (" + std::to_string(sid) + ") " +
				"to program (" + std::to_string(_id) + ") " + _name + ".");
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

			LOGE("Couldn't link shader:\n");
			LOGE("%s\n", _name.c_str());
			LOGE("Linkage log:\n");
			LOGE("%s\n", strLog.c_str());

			delete[] log;

			// Detach shaders
			for (GLuint sid : _shaders) {
				glDetachShader(_id, sid);
			}

			// Delete the program
			glDeleteProgram(_id);

			return false;
		}

		// Detach (or delete?) shaders after successful linking
		for (GLuint sid : _shaders) {
			glDetachShader(_id, sid);
		}

		return true;
	}
}