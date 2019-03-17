#ifndef __RSM_MESH_H__
#define __RSM_MESH_H__

#include "OpenGL.h"
#include "RSMaths.h"
//#include "Resources.h"

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

		Mesh(const std::string filepath);
		~Mesh();

		const GLuint& VAO() const;
		const std::vector<Vertex>& vertices() const;
		const std::vector<int>& indices() const;

		void addVertex(const Vertex& vertex);
		void addIndex(const int& idx);

		void setVAO(const GLuint& vao);
		void setVertices(const std::vector<Vertex>& vertices);
		void setIndices(const std::vector<int>& indices);

		bool loadObj(const std::string filepath);

		//void computeTangents();

	private:
		GLuint _VAO;

		std::vector<Vertex> _vertices;
		std::vector<int> _indices;
	};
}

#endif // !__RSM_MESH_H__
