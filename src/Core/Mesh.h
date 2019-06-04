#ifndef __RSM_MESH_H__
#define __RSM_MESH_H__

#include "OpenGL.h"
#include "Math/RSMaths.h"
#include "Graphics/Material.h"
//#include "Resources.h"

using namespace glm;

namespace rsm {

	struct Vertex {
		vec3 position;
		vec3 normal;
		vec2 uv;
		vec3 tangent;
	};

	class Mesh {
	public:

		Mesh();
		~Mesh();

		const std::string name() const;
		const GLuint& VAO() const;
		std::vector<Vertex>& vertices();
		std::vector<int>& indices();
		const sref<Material>& material() const;

		void addVertex(const Vertex& vertex);
		void addIndex(const int& idx);

		void setName(const std::string& name);
		void setVAO(const GLuint& vao);
		void setVertices(const std::vector<Vertex>& vertices);
		void setIndices(const std::vector<int>& indices);
		void setMaterial(sref<Material> material);

		void prepare();
		void render(glm::mat4& objToWorld, glm::mat3 normalMatrix);
		void draw(glm::mat4& objToWorld, glm::mat3 normalMatrix, GLuint program);

	private:
		std::string _name;

		GLuint _VAO;

		std::vector<Vertex> _vertices;
		std::vector<int> _indices;

		sref<Material> _material;
	};
}

#endif // !__RSM_MESH_H__
