#include "App/OpenGLApplication.h"

#include <android/asset_manager_jni.h>

#include <time.h>
#include <sys/types.h>
#include <dirent.h>

using namespace rsm;

rsm::OpenGLApplication* glApp;
AAssetManager* assetManager;

long oldTimeSinceStart;

char* getAssetSource(const char* filepath);
size_t getAssetLength(const char* filepath);
long getCurrentTime();
bool endsWith (std::string const &fullString, std::string const &ending);
void loadTextures(std::string directory, std::string extension, std::string prefix);

/** ==================================================================================
            README™ - A Comprehensive Guide on how not to fuck up!
=====================================================================================*/

//FIXME: On Reshape/Screen orientation change (Landscape <-> Portrait) "init()" seems to be called everytime!

/**
 * IMPORTANT: SUPPOSEDLY reshape is always called at least once...
 * So I'm just using useless values: 1920x1080
 * UPDATE: So far it seems to be true!
 */

void init() {

    glApp = new OpenGLApplication(1920, 1080);

    LOG("Initializing AndroidApp...\n");

    /** ==================================================================================
				Loading and Setup
	=====================================================================================*/
    glApp->init();
    RM.init();

    checkOpenGLError("Error during engine initialization!");
    LOG("Init successful...\n");

    /** ==================================================================================
				Shaders
	=====================================================================================*/

    ShaderSource vBP = ShaderSource(VERTEX_SHADER, "vBP", getAssetSource("shaders/BlinnPhong.vs"));
    ShaderSource fBP = ShaderSource(FRAGMENT_SHADER, "fBP", getAssetSource("shaders/BlinnPhong.fs"));
    vBP.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    fBP.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n") +
               std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
    vBP.compile();
    fBP.compile();

    ShaderSource vBPTex = ShaderSource(VERTEX_SHADER, "vBPTex", getAssetSource("shaders/BlinnPhongTex.vs"));
    ShaderSource fBPTex = ShaderSource(FRAGMENT_SHADER, "fBPTex", getAssetSource("shaders/BlinnPhongTex.fs"));
    vBPTex.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    fBPTex.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n") +
               std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
    vBPTex.compile();
    fBPTex.compile();

    ShaderSource vSM = ShaderSource(VERTEX_SHADER, "vSM",getAssetSource("shaders/ShadowMap.vs"));
    ShaderSource fSM = ShaderSource(FRAGMENT_SHADER, "fSM", getAssetSource("shaders/ShadowMap.fs"));
    vSM.inject(std::string("#version 320 es\n"));
    fSM.inject(std::string("#version 320 es\n"));
    vSM.compile();
    fSM.compile();

    ShaderSource vOSM = ShaderSource(VERTEX_SHADER, "vOSM", getAssetSource("shaders/OmniShadowMap.vs"));
    ShaderSource gOSM = ShaderSource(GEOMETRY_SHADER, "gOSM", getAssetSource("shaders/OmniShadowMap.gs"));
    ShaderSource fOSM = ShaderSource(FRAGMENT_SHADER, "fOSM", getAssetSource("shaders/OmniShadowMap.fs"));
    vOSM.inject(std::string("#version 320 es\n"));
    gOSM.inject(std::string("#version 320 es\n"));
    fOSM.inject(std::string("#version 320 es\n") + std::string("precision highp float;\n"));
    vOSM.compile();
    gOSM.compile();
    fOSM.compile();

    sref<Shader> BlinnPhong = make_sref<Shader>("BlinnPhong");
    BlinnPhong->addShader(vBP);
    BlinnPhong->addShader(fBP);
    BlinnPhong->link();
    RM.addShader("BlinnPhong", BlinnPhong);
    glApp->addProgram(BlinnPhong->id());

    sref<Shader> BlinnPhongTex = make_sref<Shader>("BlinnPhongTex");
    BlinnPhongTex->addShader(vBPTex);
    BlinnPhongTex->addShader(fBPTex);
    BlinnPhongTex->link();
    RM.addShader("BlinnPhongTex", BlinnPhongTex);
    glApp->addProgram(BlinnPhongTex->id());

    sref<Shader> ShadowMap = make_sref<Shader>("ShadowMap");
    ShadowMap->addShader(vSM);
    ShadowMap->addShader(fSM);
    ShadowMap->link();
    RM.addShader("ShadowMap", ShadowMap);
    //glApp->addProgram(ShadowMap->id());

    sref<Shader> OmniShadowMap = make_sref<Shader>("OmniShadowMap");
    OmniShadowMap->addShader(vOSM);
    OmniShadowMap->addShader(gOSM);
    OmniShadowMap->addShader(fOSM);
    OmniShadowMap->link();
    RM.addShader("OmniShadowMap", OmniShadowMap);
    //glApp->addProgram(OmniShadowMap->id());

    checkOpenGLError("Error during shader loading and setup!");
    LOG("Shaders loaded...\n");

    /** ==================================================================================
            Textures
    =====================================================================================*/

    /** /
    loadTextures("models/crytek sponza/textures", "png", "textures\\");
    /**/

    checkOpenGLError("Error during texture loading!");
    LOG("Textures loaded...\n");

    /** ==================================================================================
            Models
	=====================================================================================*/

    sref<Model> demo_scene = make_sref<Model>("demo_scene");
    demo_scene->loadFromMemory(getAssetSource("models/demo scene open/demo_scene.obj"), getAssetSource("models/demo scene open/demo_scene.mtl"));
    RM.addModel("demo_scene", demo_scene);

    /** /
    sref<Model> sponza = make_sref<Model>("sponza");
    sponza->loadFromMemory(getAssetSource("models/crytek sponza/sponza.obj"), getAssetSource("models/crytek sponza/sponza.mtl"));
    RM.addModel("sponza", sponza);
    /**/

    checkOpenGLError("Error during model loading!");
    LOG("Meshes and models loaded...\n");

    checkOpenGLError("Error during loading and setup!");
    LOG("AndroidApp successfully initialized!\n");
    glApp->prepare();
    LOG("AndroidApp successfully prepared!\n");
    oldTimeSinceStart = getCurrentTime();
}

void reshape(int w, int h) {
    glApp->reshape(w,h);
}

void render() {
    // get deltaTime, send it to update
    long timeSinceStart = getCurrentTime();
    long deltaTime = timeSinceStart - oldTimeSinceStart;
    oldTimeSinceStart = timeSinceStart;

    // nano to microseconds
    float dt = float(deltaTime) * 1000.0f;
    if(dt > 0.25f)
        dt = 0.25f;

    //glApp->getCamera()->updateOrientation(0.0f, dt * 0.1f);
    //glApp->getCamera()->updateViewMatrix();
    glApp->update(dt);

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

size_t getAssetLength(const char* filepath){
    AAsset* file = AAssetManager_open(assetManager, filepath, AASSET_MODE_STREAMING);
    size_t filesize = AAsset_getLength(file);
    return filesize;
}

bool endsWith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

void loadTextures(std::string directory, std::string extension, std::string prefix){

    AAssetDir* dir = AAssetManager_openDir(assetManager, directory.c_str());
    const char* file;
    while((file = AAssetDir_getNextFileName(dir)) != NULL){
        std::string filename(file);
        if(endsWith(filename, extension)){
            std::string fullpath = directory + "/" + filename;
            Image img;
            //LOG("Filename: %s\n", filename.c_str());
            img.loadFromMemory(getAssetSource(fullpath.c_str()), getAssetLength(fullpath.c_str()), IMG_2D);
            sref<Texture> tex = make_sref<Texture>(img);
            RM.addTexture(prefix + filename, tex);
        }
    }
}

/*
 * Returns current time in nanoseconds (10^-9 sec)
 */
long getCurrentTime(){
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    // in nanoseconds
    return (now.tv_sec * 1000000000) + now.tv_nsec;
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

