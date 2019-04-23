#include "Mesh.h"

using namespace rsm;

Mesh::Mesh() { }
Mesh::~Mesh() { }

const std::string Mesh::name() const {
	return _name;
}

const GLuint& Mesh::VAO() const {
	return _VAO;
}

const std::vector<Vertex>& Mesh::vertices() const {
	return _vertices;
}
const std::vector<int>& Mesh::indices() const {
	return _indices;
}

const sref<Material>& Mesh::material() const {
	return _material;
}

void Mesh::addVertex(const Vertex& vertex) {
	_vertices.push_back(vertex);
}
void Mesh::addIndex(const int& idx) {
	_indices.push_back(idx);
}

void Mesh::setName(const std::string& name) {
	_name = name;
}

void Mesh::setVAO(const GLuint& vao) {
	_VAO = vao;
}

void Mesh::setVertices(const std::vector<Vertex>& vertices) {
	_vertices.resize(vertices.size());
	std::copy(vertices.begin(), vertices.end(), _vertices.begin());
}
void Mesh::setIndices(const std::vector<int>& indices) {
	_indices.resize(indices.size());
	std::copy(indices.begin(), indices.end(), _indices.begin());
}

void Mesh::setMaterial(sref<Material> material) {
	_material = material;
}

//upload geometry to the GPU
void Mesh::prepare() {
	glGenVertexArrays(1, &_VAO);
	glBindVertexArray(_VAO);

	// 2 VBOs per VAO: vertices + indices
	GLuint VBOs[2] = { 0, 0 };
	glGenBuffers(2, VBOs);

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * _vertices.size(), &_vertices[0], GL_STATIC_DRAW);

	// vertex buffer attribute layout
	glEnableVertexAttribArray(POSITION);
	glVertexAttribPointer(POSITION, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(Vertex), (const void*)offsetof(Vertex, position));
	glEnableVertexAttribArray(NORMAL);
	glVertexAttribPointer(NORMAL, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(Vertex), (const void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(UV);
	glVertexAttribPointer(UV, 2, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(Vertex), (const void*)offsetof(Vertex, uv));

	//glEnableVertexAttribArray(TANGENT);
	//glVertexAttribPointer(TANGENT, 3, GL_FLOAT, GL_FALSE, (GLsizei)sizeof(Vertex), (const void*)offsetof(Vertex, tangent));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOs[1]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * _indices.size(), &_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Mesh::draw(glm::mat4& objToWorld, glm::mat3 normalMatrix) {
	glUseProgram(_material->program());

	glUniformMatrix4fv(glGetUniformLocation(_material->program(), "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(objToWorld));
	glUniformMatrix3fv(glGetUniformLocation(_material->program(), "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

	_material->uploadData();

	glBindVertexArray(_VAO);
	glDrawArrays(GL_TRIANGLES, 0, _vertices.size());
	glBindVertexArray(0);

	glUseProgram(0);
}