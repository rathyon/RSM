#include "Model.h"
#include <cstddef>

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\ext\tinyobj\tiny_obj_loader.h"

using namespace rsm;

Model::Model(const std::string& name) : _name(name) { }

Model::Model(const std::string& name, const vec3& position) : SceneObject(position), _name(name) { }
Model::Model(const std::string& name, const mat4& objToWorld) : SceneObject(objToWorld), _name(name) { }

Model::~Model() { }

void Model::loadFromFile(const std::string& filepath) {
	loadObj(true, filepath.c_str());
}
void Model::loadFromMemory(const char* source) {
	loadObj(false, source);
}

void Model::setMaterial(sref<Material> material) {
	for (sref<Mesh> mesh : _meshes) {
		mesh->setMaterial(material);
	}
}

std::vector<sref<Mesh>>& Model::meshes() {
	return _meshes;
}

const mat3& Model::normalMatrix() const {
	return _normalMatrix;
}

void Model::updateMatrix() {
	SceneObject::updateMatrix();
	_normalMatrix = glm::transpose(glm::inverse(mat3(_objToWorld)));
}

void Model::prepare() {
	for (sref<Mesh> mesh : _meshes) {
		mesh->prepare();
	}
}

void Model::draw() {
	updateMatrix();
	for (sref<Mesh> mesh : _meshes) {
		mesh->draw(objToWorld(), normalMatrix());
	}
}


bool Model::loadObj(bool fromFile, const char* source) {
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;


	// When calling LoadObj, triangulate is on by default, if I want to not use triangulation,
	// the Vertex creation needs to be changed: I assume all faces are triangles, check num_face_vertices
	bool successful;
	if (fromFile) {
		successful = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, source);
	}
	else {
		std::istringstream sourcestream(source);
		successful = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &sourcestream);
	}

	if (!successful) {
		LOGE("Failed to load obj!\n");
		LOGE("%s\n", _name.c_str());
		LOGE("Error message:\n");
		LOGE("%s\n", err.c_str());
		return false;
	}

	if (warn.size() > 0) {
		LOG("Tinyobj warning:\n");
		LOG("%s\n", warn.c_str());
	}

	// Shape == Mesh
	// loop over each shape
	for (tinyobj::shape_t shape : shapes) {
		int vcount = 0;
		sref<Mesh> mesh =  make_sref<Mesh>();
		mesh->setName(shape.name);

		/*
		std::vector<int> mats;
		for (int mat : shape.mesh.material_ids) {
			if (std::find(mats.begin(), mats.end(), mat) == mats.end()) {
				mats.push_back(mat);
			}
		}
		*/

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

			mesh->addVertex(vertex);
			mesh->addIndex(vcount);
			vcount++;
		}

		_meshes.push_back(mesh);
	}

	return true;
}