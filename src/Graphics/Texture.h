#ifndef __RSM_TEXTURE_H__
#define __RSM_TEXTURE_H__

#include "Core\OpenGL.h"
#include "Image.h"

/*
	Possible upgrades: 
		- Manually set filter and wrap modes
		- Varied constructors
*/

namespace rsm {

	enum TexWrapMode {
		REPEAT          = 0,
		MIRRORED_REPEAT = 1,
		CLAMP_EDGE      = 2,
		CLAMP_BORDER    = 3
	};

	enum TexFilterMode {
		NEAREST                = 0,
		LINEAR                 = 1,
		NEAREST_MIPMAP_NEAREST = 2,
		LINEAR_MIPMAP_NEAREST  = 3,
		NEAREST_MIPMAP_LINEAR  = 4,
		LINEAR_MIPMAP_LINEAR   = 5
	};

	class Texture {
	public:
		Texture(Image img);
		~Texture();

		ImageType type() const;
		GLuint id() const;

	private:
		ImageType _type;
		GLuint _id;
	};

}

#endif