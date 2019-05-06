#include "Model.h"
#include <cstddef>

#include "Resources.h"
#include "Graphics\Shader.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\ext\tinyobj\tiny_obj_loader.h"

using namespace rsm;

Model::Model(const std::string& name) : _name(name) { }

Model::Model(const std::string& name, const vec3& position) : SceneObject(position), _name(name) { }
Model::Model(const std::string& name, const mat4& objToWorld) : SceneObject(objToWorld), _name(name) { }

Model::~Model() { }

// im assuming mtl_basedir is always where .obj is (depends on how I organize assets and shit)
void Model::loadFromFile(const std::string& objpath, const std::string& matpath) {
	loadObj(true, objpath.c_str(), matpath.c_str());
}
void Model::loadFromMemory(const char* objsource, const char* matsource) {
	loadObj(false, objsource, matsource);
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

bool Model::loadObj(bool fromFile, const char* objsource, const char* matsource) {
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;


	// When calling LoadObj, triangulate is on by default, if I want to not use triangulation,
	// the Vertex creation needs to be changed: I assume all faces are triangles, check num_face_vertices
	bool successful;
	if (fromFile) {
		successful = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, objsource, matsource);
	}
	else {
		std::istringstream objstream(objsource);
		std::istringstream matstream(matsource);
		tinyobj::MaterialStreamReader matReader(matstream);
		successful = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, &objstream, &matReader);
	}

	if (!successful) {
		LOGE("Failed to load obj!\n");
		LOGE("%s\n", _name.c_str());
		LOGE("Error message:\n");
		LOGE("%s\n", err.c_str());
		return false;
	}

	if (materials.size() > 0) {
		// check tiny_obj_loader.h for details
		for (tinyobj::material_t material : materials) {
			sref<BlinnPhongMaterial> mat = make_sref<BlinnPhongMaterial>();
			mat->setProgram(RM.getShader("MainProgram")->id());
			mat->setAmbient(glm::vec3(material.ambient[0], material.ambient[1], material.ambient[2]));
			mat->setShininess(material.shininess);

			// diffuse values
			if (material.diffuse_texname != "") {
				Image diffuse;
				if (fromFile)
					diffuse.loadFromFile(std::string(matsource) + "/" + material.diffuse_texname, IMG_2D);
				else
					diffuse.loadFromMemory(matsource, IMG_2D);

				sref<Texture> diffTex = make_sref<Texture>(diffuse);
				RM.addTexture(material.diffuse_texname, diffTex);
				mat->setDiffuseTex(diffTex->id());
			}
			else {
				mat->setDiffuse(glm::vec3(material.diffuse[0], material.diffuse[1], material.diffuse[2]));
			}

			// specular values
			if (material.specular_texname != "") {
				Image specular;
				if (fromFile)
					specular.loadFromFile(std::string(matsource) + "/" + material.specular_texname, IMG_2D);
				else
					specular.loadFromMemory(matsource, IMG_2D);

				sref<Texture> specTex = make_sref<Texture>(specular);
				RM.addTexture(material.specular_texname, specTex);
				mat->setSpecularTex(specTex->id());
			}
			else {
				mat->setSpecular(glm::vec3(material.specular[0], material.specular[1], material.specular[2]));
			}


			// normal / bump values
			if (material.bump_texname != "") {
				Image normal;
				if (fromFile)
					normal.loadFromFile(std::string(matsource) + "/" + material.bump_texname, IMG_2D);
				else
					normal.loadFromMemory(matsource, IMG_2D);

				sref<Texture> normTex = make_sref<Texture>(normal);
				RM.addTexture(material.bump_texname, normTex);
				mat->setNormalTex(normTex->id());
			}
			RM.addMaterial(material.name, mat);
		}
	}



	if (warn.size() > 0) {
		LOG("Tinyobj warning:\n");
		LOG("%s\n", warn.c_str());
	}

	// load meshes
	// Shape == Mesh
	for (tinyobj::shape_t shape : shapes) {
		int vcount = 0;
		sref<Mesh> mesh =  make_sref<Mesh>();
		mesh->setName(shape.name);

		// load textures (into resources too) automatically
		// assign textures automatically

		// im just loading the first material for a given mesh, since it loads mtl PER FACE
		if (materials.size() > 0) {
			int matIdx = shape.mesh.material_ids[0];
			tinyobj::material_t mat = materials[matIdx];
			mesh->setMaterial(RM.getMaterial(mat.name));
		}

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