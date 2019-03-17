#include "Model.h"
#include <cstddef>

using namespace rsm;

Model::Model() { }

Model::Model(const sref<Mesh>& mesh) : SceneObject(), _mesh(mesh) { }
Model::Model(const vec3& position) : SceneObject(position) { }
Model::Model(const sref<Mesh>& mesh, const vec3& position) : SceneObject(position), _mesh(mesh) { }

Model::~Model() { }

const sref<Mesh>& Model::mesh() const {
	return _mesh;
}

void Model::setMesh(const sref<Mesh>& mesh) {
	_mesh = mesh;
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

	//TODO material, shader program integration

	glBindVertexArray(_mesh->VAO());

	glDrawArrays(GL_TRIANGLES, 0, _mesh->vertices().size());

	glBindVertexArray(0);

}