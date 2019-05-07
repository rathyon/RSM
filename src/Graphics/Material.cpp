#include "Material.h"

using namespace rsm;

GLuint Material::program() const {
	return _prog;
}

void Material::setProgram(const GLuint& prog) {
	_prog = prog;
}

void Material::setInt(const std::string& name, int value) {
	glUniform1i(glGetUniformLocation(_prog, name.c_str()), value);
}

void Material::setFloat(const std::string& name, float value) {
	glUniform1f(glGetUniformLocation(_prog, name.c_str()), value);
}

void Material::setVec3(const std::string& name, vec3& vec) {
	glUniform3fv(glGetUniformLocation(_prog, name.c_str()), 1, value_ptr(vec));
}

void Material::setVec4(const std::string& name, vec4& vec) {
	glUniform4fv(glGetUniformLocation(_prog, name.c_str()), 1, value_ptr(vec));
}

void Material::setMat3(const std::string& name, mat3& mat) {
	glUniformMatrix3fv(glGetUniformLocation(_prog, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}

void Material::setMat4(const std::string& name, mat4& mat) {
	glUniformMatrix4fv(glGetUniformLocation(_prog, name.c_str()), 1, GL_FALSE, value_ptr(mat));
}

void Material::setSampler(const std::string& name, int id) {
	glUniform1i(glGetUniformLocation(_prog, name.c_str()), id);
}