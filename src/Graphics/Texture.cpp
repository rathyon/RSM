#include "Texture.h"

using namespace rsm;

// IMPORTANT NOTE: in the future, import format and datatype from Image, like GL_RGB vs GL_RGBA, etc...

Texture::Texture(Image img) {
	_type = img.type();
	glGenTextures(1, &_id);

	glBindTexture(OpenGLTexTargets[_type], _id);

	if (_type == IMG_1D) {

	}
	else if (_type == IMG_2D) {
		glTexImage2D(OpenGLTexTargets[_type],
			0, 
			OpenGLInternalPixelFormats[img.channels()],
			img.width(), 
			img.height(),
			0, 
			OpenGLPixelFormats[img.channels()], 
			GL_UNSIGNED_BYTE, 
			img.data());
		checkOpenGLError("Error in glTexImage2D");

		glGenerateMipmap(OpenGLTexTargets[_type]);
		checkOpenGLError("Error in generating mipmaps");
	}
	else if (_type == IMG_3D) {

	}
	else if (_type == IMG_CUBE) {

	}

	glTexParameteri(OpenGLTexTargets[_type], GL_TEXTURE_WRAP_S, OpenGLTexWrapping[REPEAT]);
	glTexParameteri(OpenGLTexTargets[_type], GL_TEXTURE_WRAP_T, OpenGLTexWrapping[REPEAT]);
	glTexParameteri(OpenGLTexTargets[_type], GL_TEXTURE_MIN_FILTER, OpenGLTexFilters[LINEAR_MIPMAP_LINEAR]);
	glTexParameteri(OpenGLTexTargets[_type], GL_TEXTURE_MAG_FILTER, OpenGLTexFilters[LINEAR]);

	glBindTexture(OpenGLTexTargets[_type], 0);
}

Texture::~Texture() { }

ImageType Texture::type() const {
	return _type;
}

GLuint Texture::id() const {
	return _id;
}