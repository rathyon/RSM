#include "App/OpenGLApplication.h"

#include <android/asset_manager_jni.h>

using namespace rsm;

rsm::OpenGLApplication* glApp;
AAssetManager* assetManager;

char* getAssetSource(const char* filepath);

void init() {
    /**
     * IMPORTANT: SUPPOSEDLY reshape is always called at least once...
     * So I'm just using useless values: 1920x1080
     * UPDATE: So far it seems to be true!
     */
    glApp = new OpenGLApplication(1920, 1080);

    LOG("Initializing AndroidApp...\n");

    /* ===================================================================================
				Loading and Setup
	=====================================================================================*/
    glApp->init();
    RM.init();

    LOG("Init successful...\n");

    /* ===================================================================================
				Shaders and Materials
	=====================================================================================*/

    ShaderSource bpV = ShaderSource(VERTEX_SHADER, "bpV", getAssetSource("shaders/BlinnPhong.vs"));
    ShaderSource bpF = ShaderSource(FRAGMENT_SHADER, "bpF", getAssetSource("shaders/BlinnPhong.fs"));

    bpV.inject("#version 320 es\n"
               "#extension GL_EXT_shader_io_blocks : enable\n");
    bpF.inject("#version 320 es\n"
               "#extension GL_EXT_shader_io_blocks : enable\n"
               "\n"
               "precision highp float;");

    bpV.compile();
    bpF.compile();

    sref<Shader> program = make_sref<Shader>("MainProgram");
    program->addShader(bpV);
    program->addShader(bpF);

    program->link();

    RM.addShader("MainProgram", program);
    glApp->addProgram(program->id());

    sref<BlinnPhongMaterial> bp_test = make_sref<BlinnPhongMaterial>();
    bp_test->setDiffuse(glm::vec3(1.0f, 0.5f, 0.2f));
    bp_test->setSpecular(glm::vec3(1.0f));
    bp_test->setShininess(32.0f);

    bp_test->setProgram(program->id());

    LOG("Shaders and materials loaded...\n");
    /* ===================================================================================
				Meshes and Models
	=====================================================================================*/

    /** /
    sref<Mesh> cube_mesh = make_sref<Mesh>("cube", getAssetSource("models/cube.obj"));
    sref<Model> test_cube = make_sref<Model>(cube_mesh);
    test_cube->setMaterial(bp_test);
    RM.addMesh("cube_mesh", cube_mesh);
    RM.addModel("test_cube", test_cube);
    /**/

    /** /
	sref<Mesh> bunny_mesh = make_sref<Mesh>("bunny", getAssetSource("models/bunny.obj"));
	sref<Model> bunny = make_sref<Model>(bunny_mesh);
	bunny->setMaterial(bp_test);
	RM.addMesh("bunny_mesh", bunny_mesh);
	RM.addModel("bunny", bunny);
	/**/

    /**/
    sref<Mesh> sponza_mesh = make_sref<Mesh>("sponza", getAssetSource("models/sponza.obj"));
    sref<Model> sponza = make_sref<Model>(sponza_mesh);
    sponza->setMaterial(bp_test);
    RM.addMesh("sponza_mesh", sponza_mesh);
    RM.addModel("sponza", sponza);
    /**/

    LOG("Meshes and models loaded...\n");
    /* ===================================================================================
				Textures
	=====================================================================================*/

    // Read files, etc... input stuff
    // ...

    checkOpenGLError("Error during loading and setup!");
    LOG("AndroidApp successfully initialized!\n");
    glApp->prepare();
    LOG("AndroidApp successfully prepared!\n");
}

void reshape(int w, int h) {
    glApp->reshape(w,h);
}

void render() {
    // get deltaTime, send it to update
    glApp->update(0.0f);
    glApp->render();
}

char* getAssetSource(const char* filepath) {
    AAsset* file = AAssetManager_open(assetManager, filepath, AASSET_MODE_STREAMING);
    size_t filesize = AAsset_getLength(file);

    char* filesource = new char[filesize+1];
    AAsset_read(file, filesource, filesize);
    filesource[filesize] = '\0';

    return filesource;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_init(JNIEnv *env, jclass type, jobject aMgr);
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_resize(JNIEnv *env, jclass type, jint width, jint height);
    JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_render(JNIEnv *env, jclass type);
};

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_init(JNIEnv* env, jclass type, jobject aMgr) {
    assetManager = AAssetManager_fromJava(env, aMgr);
    init();
}

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_resize(JNIEnv* env, jclass type, jint width, jint height) {
    reshape(width, height);
}

JNIEXPORT void JNICALL Java_com_rbenjamim_rsm_AndroidApp_render(JNIEnv* env, jclass type) {
    render();
}

