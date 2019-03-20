#include "Model.h"
#include <cstddef>

using namespace rsm;

Model::Model() { }

Model::Model(const sref<Mesh>& mesh) : SceneObject(), _mesh(mesh), _material(nullptr) { }
Model::Model(const vec3& position) : SceneObject(position), _material(nullptr) { }
Model::Model(const sref<Mesh>& mesh, const vec3& position) : SceneObject(position), _mesh(mesh), _material(nullptr) { }
Model::Model(const mat4& objToWorld) : SceneObject(objToWorld), _material(nullptr) { }

Model::~Model() { }

const sref<Mesh>& Model::mesh() const {
	return _mesh;
}

const sref<Material>& Model::material() const {
	return _material;
}

const mat3& Model::normalMatrix() const {
	return _normalMatrix;
}

void Model::updateMatrix() {
	SceneObject::updateMatrix();
	_normalMatrix = glm::transpose(glm::inverse(mat3(_objToWorld)));
}

void Model::setMesh(const sref<Mesh>& mesh) {
	_mesh = mesh;
}

void Model::setMaterial(const sref<Material>& material) {
	_material = material;
}

//upload geometry to the GPU
void Model::prepare() {
	
	std::vector<Vertex> vertices = _mesh->vertices();
	std::vector<int> indices = _mesh->indices();

	GLuint vao;
	glGenVertexArrays(1, &vao);
	_mesh->setVAO(vao);

	glBindVertexArray(_mesh->VAO());

	// 2 VBOs per VAO: vertices + indices
	GLuint VBOs[2] = { 0, 0 };
	glGenBuffers(2, VBOs);

	// vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

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
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void Model::draw() {
	updateMatrix();

	glUseProgram(_material->program());

	glUniformMatrix4fv(glGetUniformLocation(_material->program(), "ModelMatrix"), 1, GL_FALSE, glm::value_ptr(objToWorld()));
	glUniformMatrix3fv(glGetUniformLocation(_material->program(), "NormalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix()));

	_material->uploadData();

	glBindVertexArray(_mesh->VAO());

	glDrawArrays(GL_TRIANGLES, 0, _mesh->vertices().size());

	glBindVertexArray(0);

	glUseProgram(0);
}