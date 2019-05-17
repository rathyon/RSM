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
long getCurrentTime();
bool endsWith (std::string const &fullString, std::string const &ending);
void loadTextures(std::string directory, std::string extension, std::string prefix);

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
				Shaders
	=====================================================================================*/

    ShaderSource bpV = ShaderSource(VERTEX_SHADER, "bpV", getAssetSource("shaders/BlinnPhong.vs"));
    ShaderSource bpF = ShaderSource(FRAGMENT_SHADER, "bpF", getAssetSource("shaders/BlinnPhong.fs"));
    bpV.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    bpF.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n") +
               std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
    bpV.compile();
    bpF.compile();

    ShaderSource bptV = ShaderSource(VERTEX_SHADER, "bptV", getAssetSource("shaders/BlinnPhongTex.vs"));
    ShaderSource bptF = ShaderSource(FRAGMENT_SHADER, "bptF", getAssetSource("shaders/BlinnPhongTex.fs"));
    bptV.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    bptF.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n") +
               std::string("const int NUM_LIGHTS = ") + std::to_string(NUM_LIGHTS) + ";\n");
    bptV.compile();
    bptF.compile();

    sref<Shader> BlinnPhong = make_sref<Shader>("BlinnPhong");
    BlinnPhong->addShader(bpV);
    BlinnPhong->addShader(bpF);
    BlinnPhong->link();
    RM.addShader("BlinnPhong", BlinnPhong);
    glApp->addProgram(BlinnPhong->id());

    sref<Shader> BlinnPhongTex = make_sref<Shader>("BlinnPhongTex");
    BlinnPhongTex->addShader(bptV);
    BlinnPhongTex->addShader(bptF);
    BlinnPhongTex->link();
    RM.addShader("BlinnPhongTex", BlinnPhongTex);
    glApp->addProgram(BlinnPhongTex->id());

    LOG("Shaders loaded...\n");

    /* ===================================================================================
            Textures
    =====================================================================================*/
    // Read Textures, store them in RM so that loadFromMemory works for Models
    // TODO: test loadTextures
    Image testimg;
    testimg.loadFromMemory(getAssetSource("models/crytek sponza/textures/background.png"), IMG_2D);
    sref<Texture> testtex = make_sref<Texture>(testimg);
    RM.addTexture("testtex", testtex);


    loadTextures("models/crytek sponza/textures", "png", "textures\\");


    LOG("Textures loaded...\n");
    /* ===================================================================================
				Meshes and Models
	=====================================================================================*/

    /**/
    sref<Model> sponza = make_sref<Model>("sponza");
    sponza->loadFromMemory(getAssetSource("models/crytek sponza/sponza.obj"), getAssetSource("models/crytek sponza/sponza.mtl"));
    RM.addModel("sponza", sponza);
    /**/

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
            Image img;
            //img.loadFromMemory(getAssetSource(filename.c_str()), IMG_2D);
            img.loadFromFile(directory + "/" + filename, IMG_2D);
            sref<Texture> tex = make_sref<Texture>(img);
            RM.addTexture(prefix + filename, tex);
            LOG("Texture added: %s\n", filename.c_str());
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

