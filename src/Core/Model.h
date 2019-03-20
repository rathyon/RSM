#ifndef __RSM_MODEL_H__
#define __RSM_MODEL_H__

#include "SceneObject.h"
#include "Mesh.h"
#include "Material.h"

namespace rsm {

	class Model : public SceneObject {
	public:
		Model();
		Model(const sref<Mesh>& mesh);
		Model(const vec3& position);
		Model(const sref<Mesh>& mesh, const vec3& position);
		Model(const mat4& objToWorld);
		~Model();

		const sref<Mesh>& mesh() const;
		const sref<Material>& material() const;
		const mat3& normalMatrix() const;

		void setMesh(const sref<Mesh>& mesh);
		void setMaterial(const sref<Material>& material);

		void updateMatrix() override;

		void prepare();
		void draw();

	private:
		sref<Mesh> _mesh;
		sref<Material> _material;

		mat3 _normalMatrix;
	};

}

#endif