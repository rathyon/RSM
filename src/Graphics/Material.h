#ifndef __RSM_MATERIAL_H__
#define __RSM_MATERIAL_H__

#include "RSM.h"
#include "OpenGL.h"
#include "RSMaths.h"

using namespace glm;

namespace rsm {

	class Material {
	public:
		Material () { }

		GLuint program() const;

		void setProgram(const GLuint& prog);

		virtual void uploadData() = 0;

	protected:
		GLuint _prog;

		void setFloat(const std::string& name, float value);
		void setVec3(const std::string& name, vec3& vec);
		void setVec4(const std::string& name, vec4& vec);
		void setMat3(const std::string& name, mat3& mat);
		void setMat4(const std::string& name, mat4& mat);
	};

}

#endif // !__RSM_MATERIAL_H__
