#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

using namespace rsm;

Mesh::Mesh(const std::string filepath) {
	if (!loadObj(filepath)) {
		LOGE("Failed to load obj: " + filepath);
	}
	// load into resource manager here?
}

Mesh::~Mesh() {}

const GLuint& Mesh::VAO() const {
	return _VAO;
}

const std::vector<Vertex>& Mesh::vertices() const {
	return _vertices;
}
const std::vector<int>& Mesh::indices() const {
	return _indices;
}

void Mesh::addVertex(const Vertex& vertex) {
	_vertices.push_back(vertex);
}
void Mesh::addIndex(const int& idx) {
	_indices.push_back(idx);
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

bool Mesh::loadObj(const std::string filepath) {
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials; // not going to load mtl files, for now at least

	std::string warn;
	std::string err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
		LOGE("Failed to load obj " + filepath + ": " + err);
		return false;
	}

	if (warn.size() > 0)
		LOG("Tinyobj warning: " + warn);

	// When calling LoadObj, triangulate is on by default, if I want to not use triangulation,
	// the Vertex creation needs to be changed: I assume all faces are triangles, check num_face_vertices

	int vcount = 0;
	// loop over each shape
	for (tinyobj::shape_t shape : shapes) {

		// loop over each vertex
		for (tinyobj::index_t index : shape.mesh.indices) {
			Vertex vertex;

			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			if (attrib.normals.size() > 0) {
				vertex.normal = {
					attrib.normals[3 * index.normal_index + 0],
					attrib.normals[3 * index.normal_index + 1],
					attrib.normals[3 * index.normal_index + 2]
				};
			}

			if (attrib.texcoords.size() > 0) {
				vertex.uv = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					attrib.texcoords[2 * index.texcoord_index + 1]
				};
			}

			_vertices.push_back(vertex);
			_indices.push_back(vcount);
			vcount++;
		}
	}

	return true;
}