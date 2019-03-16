#ifndef __RSM_MESH_H__
#define __RSM_MESH_H__

#include "OpenGL.h"
#include "RSMaths.h"

using namespace glm;

namespace rsm {

	struct Vertex {
		vec3 position;
		vec3 normal;
		vec2 uv;
		//vec3 tangent;
	};

	class Mesh {
	public:

		Mesh();
		Mesh(const std::string filepath);
		~Mesh();

		const std::vector<Vertex>& vertices() const;
		const std::vector<uint>& indices() const;

		void addVertex(const Vertex& vertex);
		void addIndex(const uint& idx);

		void setVertices(const std::vector<Vertex>& vertices);
		void setIndices(const std::vector<uint>& indices);

		bool loadObj(const std::string filepath);

		//void computeTangents();

	private:
		std::vector<Vertex> _vertices;
		std::vector<uint> _indices;
	};
}

#endif // !__RSM_MESH_H__
