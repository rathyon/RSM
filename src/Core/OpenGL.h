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

void checkOpenGLError(const std::string& error);
bool isOpenGLError();

#endif
