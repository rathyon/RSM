#include "OpenGL.h"

const char *getGLErrorString(GLenum err)
{
    switch (err) {
        case GL_NO_ERROR:
            return "No error";
        case GL_INVALID_ENUM:
            return "Invalid enum";
        case GL_INVALID_VALUE:
            return "Invalid value";
        case GL_INVALID_OPERATION:
            return "Invalid operation";
        case GL_STACK_OVERFLOW:
            return "Stack overflow";
        case GL_STACK_UNDERFLOW:
            return "Stack underflow";
        case GL_OUT_OF_MEMORY:
            return "Out of memory";
        default:
            return "Unknown error";
    }
}

void checkOpenGLError(const std::string& error) {
	if (isOpenGLError()) {
		LOGE("%s\n", error.c_str());
		std::cin.get();
		exit(EXIT_FAILURE);
	}
}

bool isOpenGLError() {
	bool isError = false;
	GLenum errCode;
	const char *errString;
	while ((errCode = glGetError()) != GL_NO_ERROR) {
		isError = true;
		errString = getGLErrorString(errCode);
		LOGE("OpenGL ERROR!\n");
		LOGE("%s\n", errString);
	}
	return isError;
}