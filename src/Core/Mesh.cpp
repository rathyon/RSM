#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace rsm;

Mesh::Mesh() {}

Mesh::Mesh(const std::string filepath) {
	if (!loadObj(filepath)) {
		LOGE("Failed to load obj: " + filepath);
	}
}

Mesh::~Mesh() {}

const std::vector<Vertex>& Mesh::vertices() const {
	return _vertices;
}
const std::vector<uint>& Mesh::indices() const {
	return _indices;
}

void Mesh::addVertex(const Vertex& vertex) {
	_vertices.push_back(vertex);
}
void Mesh::addIndex(const uint& idx) {
	_indices.push_back(idx);
}

void Mesh::setVertices(const std::vector<Vertex>& vertices) {
	_vertices.resize(vertices.size());
	std::copy(vertices.begin(), vertices.end(), _vertices.begin());
}
void Mesh::setIndices(const std::vector<uint>& indices) {
	_indices.resize(indices.size());
	std::copy(indices.begin(), indices.end(), _indices.begin());
}

bool Mesh::loadObj(const std::string filepath) {
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;

	//TODO
	return true;
}