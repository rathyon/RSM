#ifndef __RSM_MESH_H__
#define __RSM_MESH_H__

#include "OpenGL.h"
#include "Math\RSMaths.h"
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
		Mesh(const std::string name, const char* source);
		~Mesh();

		const GLuint& VAO() const;
		const std::vector<Vertex>& vertices() const;
		const std::vector<int>& indices() const;

		void addVertex(const Vertex& vertex);
		void addIndex(const int& idx);

		void setVAO(const GLuint& vao);
		void setVertices(const std::vector<Vertex>& vertices);
		void setIndices(const std::vector<int>& indices);

		//void computeTangents();

	private:
		std::string _name;

		GLuint _VAO;

		std::vector<Vertex> _vertices;
		std::vector<int> _indices;

		bool loadObj(bool fromFile, const char* source);
	};
}

#endif // !__RSM_MESH_H__
