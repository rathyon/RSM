#ifndef __RSM_OPENGL_H__
#define __RSM_OPENGL_H__

#ifdef __ANDROID__
#include <jni.h>
#include <android/log.h>
#include <GLES3/gl32.h>
#include <GLES3/gl3ext.h>
#include <EGL/egl.h>

#define LOG(str) __android_log_print(ANDROID_LOG_INFO, "%s", str)

#else
#include <iostream>
#include <GL/glew.h>
#include <GL/freeglut.h>

#define LOG(str) std::cout << str << std::endl
#endif

void checkOpenGLError(const std::string& error);
bool isOpenGLError();

#endif
