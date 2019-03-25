#ifndef __RSM_BLINNPHONG_H__
#define __RSM_BLINNPHONG_H__

#include "Material.h"

using namespace glm;

namespace rsm {

	class BlinnPhongMaterial : public Material {
	public:
		BlinnPhongMaterial();
		//TODO constructor with all parameters


		void uploadData();

		void setAmbient(const vec3& ambient);

		void setDiffuse(const vec3& diffuse);
		//void setDiffuse(const Texture& diffTex);

		void setSpecular(const vec3& specular);
		//void setNormal(const Texture& normalTex);

		void setShininess(float shininess);

		vec3 ambient() const;
		vec3 diffuse() const;
		vec3 specular() const;
		float shininess() const;

		//Texture diffuseTex() const;
		//Texture normalTex() const;

	private:
		vec3 _ambient;
		vec3 _diffuse;
		vec3 _specular;
		float _shininess;

		//Texture _diffuseTex;
		//Texture _specularTex;
	};

}

#endif // !__RSM_BLINNPHONG_H__
