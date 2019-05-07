#ifndef __RSM_BLINNPHONG_H__
#define __RSM_BLINNPHONG_H__

#include "Material.h"

using namespace glm;

namespace rsm {

	class BlinnPhongMaterial : public Material {
	public:
		BlinnPhongMaterial();
		//TODO constructor with all parameters?


		void uploadData();

		void setAmbient(const vec3& ambient);

		void setDiffuse(const vec3& diffuse);
		void setDiffuseTex(GLuint diffTex);

		void setSpecular(const vec3& specular);
		void setSpecularTex(GLuint specTex);

		void setNormalMap(GLuint normalMap);

		void setShininess(float shininess);

		vec3 ambient() const;
		vec3 diffuse() const;
		vec3 specular() const;
		float shininess() const;

		GLuint diffuseTex() const;
		GLuint specularTex() const;
		GLuint normalMap() const;

	private:
		vec3 _ambient;
		vec3 _diffuse;
		vec3 _specular;
		float _shininess;

		GLuint _diffuseTex;
		GLuint _specularTex;
		GLuint _normalMap;
	};

}

#endif // !__RSM_BLINNPHONG_H__
