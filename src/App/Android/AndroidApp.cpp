#include "Core/OpenGLApplication.h"

using namespace rsm;

// MOBILE IMPORTING

rsm::OpenGLApplication* glApp;

void init() {
    glApp = new OpenGLApplication();
    glApp->init();

    /** /
    RM.init();

    Shader vShader = Shader(VERTEX_SHADER, "vertex.vs");
    vShader.inject("#version 330 core\n");

    Shader fShader = Shader(FRAGMENT_SHADER, "frag.fs");
    fShader.inject("#version 330 core\n");

    sref<Shader> program = make_sref<Shader>("ShaderProgram");

    vShader.compile();
    fShader.compile();

    program->attach(vShader);
    program->attach(fShader);

    RM.addShader("program", program);
    /**/

    // Read files, etc... input stuff
    // ...
    glApp->prepare();
}

void reshape(int w, int h) {
    glApp->reshape(w,h);
}

void render() {
    // get deltaTime, send it to update
    //glApp->update();
    glApp->render();
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_init(JNIEnv *env, jclass type);
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_resize(JNIEnv *env, jclass type, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_render(JNIEnv *env, jclass type);
};

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_init(JNIEnv *env, jclass type) {
    init();
}

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_resize(JNIEnv *env, jclass type, jint width, jint height) {
    reshape(width, height);
}

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_render(JNIEnv *env, jclass type) {
    render();
}

