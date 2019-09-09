#ifndef __RSM_OPENGL_H__
#define __RSM_OPENGL_H__

#include <RSM.h>

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>

#else
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#endif

// ive decided I will only have 1 light AT ANY TIME
//#define RSM_NAIVE
#define RSM_DEFERRED

#define NUM_VPL 100

#define LOW_RES_INDIRECT_WIDTH 256
#define LOW_RES_INDIRECT_HEIGHT 256

// Lucy Test Scene:
#define VPL_DIST_MAX 0.3f
#define RSM_INTENSITY 3.0f


enum TextureUnit {
	DIFFUSE_MAP  = 0,
	SPECULAR_MAP = 1,
	NORMAL_MAP   = 2,

	G_POSITION           = 3,
	G_NORMAL             = 4,
	G_DIFFUSE            = 5,
	G_SPECULAR           = 6,
	G_LIGHTSPACEPOSITION = 7,

	RSM_DEPTH    = 8,
	RSM_POSITION = 9,
	RSM_NORMAL   = 10,
	RSM_FLUX     = 11,

	LOW_RES_INDIRECT = 12,
};

const GLenum OpenGLTextureUnits[]{
	GL_TEXTURE0,
	GL_TEXTURE1,
	GL_TEXTURE2,
	GL_TEXTURE3,
	GL_TEXTURE4,
	GL_TEXTURE5,
	GL_TEXTURE6,
	GL_TEXTURE7,
	GL_TEXTURE8,
	GL_TEXTURE9,
	GL_TEXTURE10,
	GL_TEXTURE11,
	GL_TEXTURE12,
	GL_TEXTURE13,
};

const GLenum OpenGLShaderType[]{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	GL_GEOMETRY_SHADER,
	GL_COMPUTE_SHADER
};

// Exists because of OpenGL ES 3.0's API regarding glTexImage2D
const GLenum OpenGLInternalPixelFormats[] = {
		0,
		GL_R8,
		GL_RG8,
		GL_RGB,
		GL_RGBA
};

const GLenum OpenGLPixelFormats[] = {
	0,
	GL_RED,
	GL_RG,
	GL_RGB,
	GL_RGBA
};

// OpenGL ES 3.2 doesn't support GL_TEXTURE_1D !
const GLenum OpenGLTexTargets[] = {
	0,
	GL_TEXTURE_2D,
	GL_TEXTURE_3D,
	GL_TEXTURE_CUBE_MAP,
	GL_TEXTURE_CUBE_MAP_POSITIVE_X,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
	GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
	GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

const GLenum OpenGLTexFilters[] = {
	GL_NEAREST,
	GL_LINEAR,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_NEAREST,
	GL_NEAREST_MIPMAP_LINEAR,
	GL_LINEAR_MIPMAP_LINEAR
};

const GLenum OpenGLTexWrapping[] = {
	GL_REPEAT,
	GL_MIRRORED_REPEAT,
	GL_CLAMP_TO_EDGE,
	GL_CLAMP_TO_BORDER
};

const enum ShaderType {
	VERTEX_SHADER     = 0,
	FRAGMENT_SHADER   = 1,
	GEOMETRY_SHADER   = 2,
	COMPUTE_SHADER    = 3,
	SHADER_TYPE_COUNT = 4
};

const enum AttributeType {
	POSITION  = 0,
	NORMAL    = 1,
	UV        = 2,
	TANGENT   = 3,
	BITANGENT = 4
};

const enum BufferIndices {
	CAMERA_BUFFER_IDX = 0,
	LIGHTS_BUFFER_IDX = 1
};

void checkOpenGLError(const std::string& error);
bool isOpenGLError();
float randf();

#endif
