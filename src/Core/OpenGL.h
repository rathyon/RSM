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

#define NUM_LIGHTS 1

const GLenum OpenGLShaderType[]{
	GL_VERTEX_SHADER,
	GL_FRAGMENT_SHADER,
	//GL_GEOMETRY_SHADER, // OpenGL ES doesn't have it ?
	//GL_COMPUTE_SHADER	// OpenGL ES doesn't have it ?
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

#endif
