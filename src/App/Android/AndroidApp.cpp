#include "App/OpenGLApplication.h"

#include <android/asset_manager_jni.h>

#include <time.h>
#include <sys/types.h>
#include <dirent.h>

using namespace rsm;

rsm::OpenGLApplication* glApp;
AAssetManager* assetManager;

long oldTimeSinceStart;

int totalFrames = 0;
double totalTimePerFrame = 0.0f;

char* getAssetSource(const char* filepath);
size_t getAssetLength(const char* filepath);
long getCurrentTime();
bool endsWith (std::string const &fullString, std::string const &ending);
void loadTextures(std::string directory, std::string extension, std::string prefix);

/** ==================================================================================
            README™ - A Comprehensive Guide on how not to fuck up!
=====================================================================================*/

void init() {
    int width, height;
    // Screen resolutions are:
    // HD+ 1480x720
    // FHD+ 2220x1080
    // WQHD+ 2960x1440

    width = 1480; height = 720;
    //width = 2220; height = 1080;
    //width = 2960; height = 1440;

    glApp = new OpenGLApplication(width, height);

    LOG("Initializing AndroidApp...\n");

    /** ==================================================================================
				Loading and Setup
	=====================================================================================*/
    glApp->init();
    RM.init();

    /* ===================================================================================
                Conference
    =====================================================================================*/

    /** /
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-25.0f, -15.0f, 0.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width, height, 15.0f, 0.1f, 10000.0f, glm::vec3(20.0f, 16.f, -3.f), glm::vec3(-5.0f, 1.0f, -3.0f));
    /**/

    /* ===================================================================================
                Sphere and Cube
    =====================================================================================*/

    /** /
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(10.0f, -7.5f, -18.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(-7.0f, 10.f, 15.f), glm::vec3(3.0f, 2.5f, -3.0f));
    /**/

    /* ===================================================================================
                Lucy
    =====================================================================================*/

    /** /
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(10.f, 10.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
    /**/

    /* ===================================================================================
                Crytek Sponza
    =====================================================================================*/

    /**/
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-89.5f, -85.0f, -15.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width/2, height/2, 100.0f, 0.1f, 100000.0f, glm::vec3(50.f, 85.f, 5.f), glm::vec3(-39.5f, 0.f, -10.f));
    /**/

    /* ===================================================================================
                Cornell Box
    =====================================================================================*/

    /** /
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-0.5f, -0.4f, -1.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width, height, 10.0f, 0.1f, 1000.0f, glm::vec3(5.f, 4.f, 10.f), glm::vec3(0.f, 0.f, 0.f));
    /**/

    /* ===================================================================================
                Sibenik
    =====================================================================================*/

    /** /
    sref<DirectionalLight> sun = make_sref<DirectionalLight>(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, -1.0f, -1.0f));
    glApp->getScene()->addLight(sun);
    sun->prepare(width, height, 10.0f, 0.1f, 10000.0f, glm::vec3(-10.f, 10.f, 0.f), glm::vec3(15.f, 0.f, 0.f));
    /**/

    /* ===================================================================================
                Unused...
    =====================================================================================*/
    /** /
    // emission, cutoff, direction, position
    sref<SpotLight> spot = make_sref<SpotLight>(glm::vec3(2.0f, 2.0f, 2.0f), 20.0f, glm::vec3(-1.0f, -0.5f, -1.0f), glm::vec3(8.0f, 8.0f, 8.0f));
    _scene.addLight(spot);
    spot->prepare(_width, _height);
    /**/

    const sref<Light>& light = glApp->getScene()->lights()[0];
    LightData data;
    light->toData(data);
    const int lightType = data.type;

    checkOpenGLError("Error during engine initialization!");
    LOG("Init successful...\n");

    /* ===================================================================================
                Shaders
    =====================================================================================*/
    /**/


    std::string lightDef;
    if (lightType == LightType::LIGHTYPE_DIR)
        lightDef = "#define DIRECTIONAL\n";
    else
        lightDef = "#define SPOTLIGHT\n";

    ShaderSource vBP = ShaderSource(VERTEX_SHADER, "vBP", getAssetSource("shaders/BlinnPhong.vs"));
    ShaderSource fBP = ShaderSource(FRAGMENT_SHADER, "fBP", getAssetSource("shaders/BlinnPhong.fs"));
    vBP.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    fBP.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               lightDef +
               std::string("precision highp float;\n") +
               std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
    vBP.compile();
    fBP.compile();

    ShaderSource vBPT = ShaderSource(VERTEX_SHADER, "vBPT", getAssetSource("shaders/BlinnPhongTex.vs"));
    ShaderSource fBPT = ShaderSource(FRAGMENT_SHADER, "fBPT", getAssetSource("shaders/BlinnPhongTex.fs"));
    vBPT.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));

    fBPT.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               lightDef +
               std::string("precision highp float;\n") +
               std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
    vBPT.compile();
    fBPT.compile();

    ShaderSource vDS = ShaderSource(VERTEX_SHADER, "vDS", getAssetSource("shaders/DeferredShading.vs"));
    ShaderSource fDS = ShaderSource(FRAGMENT_SHADER, "fDS", getAssetSource("shaders/DeferredShading.fs"));
    vDS.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));
    fDS.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               lightDef +
               std::string("precision highp float;\n") +
               std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
    vDS.compile();
    fDS.compile();

    ShaderSource vII = ShaderSource(VERTEX_SHADER, "vII", getAssetSource("shaders/IndirectIllumination.vs"));
    ShaderSource fII = ShaderSource(FRAGMENT_SHADER, "fII", getAssetSource("shaders/IndirectIllumination.fs"));
    vII.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));
    fII.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n") +
               std::string("const int NUM_VPL = ") + std::to_string(NUM_VPL) + ";\n");
    vII.compile();
    fII.compile();

    ShaderSource vGB = ShaderSource(VERTEX_SHADER, "vGB",getAssetSource("shaders/GBuffer.vs"));
    ShaderSource fGB = ShaderSource(FRAGMENT_SHADER, "fGB", getAssetSource("shaders/GBuffer.fs"));
    vGB.inject(std::string("#version 320 es\n") +
                std::string("#extension GL_EXT_shader_io_blocks : enable\n"));
    fGB.inject(std::string("#version 320 es\n") +
                std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
                std::string("precision highp float;\n"));
    vGB.compile();
    fGB.compile();

    ShaderSource vRGB = ShaderSource(VERTEX_SHADER, "vRGB",getAssetSource("shaders/RSMGBuffer.vs"));
    ShaderSource fRGB = ShaderSource(FRAGMENT_SHADER, "fRGB", getAssetSource("shaders/RSMGBuffer.fs"));
    vRGB.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n"));
    fRGB.inject(std::string("#version 320 es\n") +
               std::string("#extension GL_EXT_shader_io_blocks : enable\n") +
               std::string("precision highp float;\n"));
    vRGB.compile();
    fRGB.compile();

    sref<Shader> BlinnPhong = make_sref<Shader>("BlinnPhong");
    BlinnPhong->addShader(vBP);
    BlinnPhong->addShader(fBP);
    BlinnPhong->link();
    RM.addShader("BlinnPhong", BlinnPhong);
    glApp->addProgram(BlinnPhong->id());

    sref<Shader> BlinnPhongTex = make_sref<Shader>("BlinnPhongTex");
    BlinnPhongTex->addShader(vBPT);
    BlinnPhongTex->addShader(fBPT);
    BlinnPhongTex->link();
    RM.addShader("BlinnPhongTex", BlinnPhongTex);
    glApp->addProgram(BlinnPhongTex->id());

    sref<Shader> DeferredShading = make_sref<Shader>("DeferredShading");
    DeferredShading->addShader(vDS);
    DeferredShading->addShader(fDS);
    DeferredShading->link();
    RM.addShader("DeferredShading", DeferredShading);
    glApp->addProgram(DeferredShading->id());

    sref<Shader> IndirectIllumination = make_sref<Shader>("IndirectIllumination");
    IndirectIllumination->addShader(vII);
    IndirectIllumination->addShader(fII);
    IndirectIllumination->link();
    RM.addShader("IndirectIllumination", IndirectIllumination);
    glApp->addProgram(IndirectIllumination->id());

    sref<Shader> GBuffer = make_sref<Shader>("GBuffer");
    GBuffer->addShader(vGB);
    GBuffer->addShader(fGB);
    GBuffer->link();
    RM.addShader("GBuffer", GBuffer);

    sref<Shader> RSMGBuffer = make_sref<Shader>("RSMGBuffer");
    RSMGBuffer->addShader(vRGB);
    RSMGBuffer->addShader(fRGB);
    RSMGBuffer->link();
    RM.addShader("RSMGBuffer", RSMGBuffer);

    checkOpenGLError("Error during shader loading and setup!");
    LOG("Shaders loaded...\n");

    /* ===================================================================================
				Test Scenes
	=====================================================================================*/

    /* ===================================================================================
                Conference
    =====================================================================================*/

    /** /
    sref<Model> Conference = make_sref<Model>("Conference");
    Conference->loadFromMemory(getAssetSource("models/Conference Modified/conference.obj"), getAssetSource("models/Conference Modified/conference.mtl"));
    RM.addModel("Conference", Conference);
    Conference->prepare();
    glApp->getScene()->addModel(Conference);

    sref<Camera> camera = make_sref<Perspective>(width, height,
        vec3(6.5f, 4.0f, -10.0f),
        vec3(-5.0f, 2.0f, -1.0f),
        vec3(0.0f, 1.0f, 0.0f),
        0.1f, 1000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    /* ===================================================================================
                Sphere and Cube
    =====================================================================================*/

    /** /
    sref<Model> SphereCube = make_sref<Model>("SphereCube");
    SphereCube->loadFromMemory(getAssetSource("models/Sphere and Cube/spherecube.obj"), getAssetSource("models/Sphere and Cube/spherecube.mtl"));
    RM.addModel("SphereCube", SphereCube);
    SphereCube->prepare();
    glApp->getScene()->addModel(SphereCube);

    sref<Camera> camera = make_sref<Perspective>(width, height,
                                                 vec3(0.0f, 5.0f, 12.0f),
                                                 vec3(0.0f, 2.5f, -1.0f),
                                                 vec3(0.0f, 1.0f, 0.0f),
                                                 0.1f, 1000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    /* ===================================================================================
                Lucy
    =====================================================================================*/

    /** /
    sref<Model> Lucy = make_sref<Model>("Lucy");
    Lucy->loadFromMemory(getAssetSource("models/Lucy/Lucy.obj"), getAssetSource("models/Lucy/Lucy.mtl"));
    RM.addModel("Lucy", Lucy);
    Lucy->prepare();
    glApp->getScene()->addModel(Lucy);

    sref<Camera> camera = make_sref<Perspective>(width, height,
        vec3(-5.0f, 3.0f, -6.0f),
        vec3(0.0f, 3.5f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        0.1f, 1000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    /* ===================================================================================
                Crytek Sponza
    =====================================================================================*/

    /**/
    loadTextures("models/crytek sponza/textures", "png", "textures\\");
    checkOpenGLError("Error during texture loading!");
    LOG("Textures loaded...\n");

    sref<Model> sponza = make_sref<Model>("sponza");
    sponza->loadFromMemory(getAssetSource("models/crytek sponza/sponza.obj"), getAssetSource("models/crytek sponza/sponza.mtl"));
    RM.addModel("sponza", sponza);
    sponza->prepare();
    sponza->setScale(0.05f, 0.05f, 0.05f);
    glApp->getScene()->addModel(sponza);

    sref<Camera> camera = make_sref<Perspective>(width, height,
        vec3(2.0f, 11.0f, -2.0f),
        vec3(-71.0f, 14.0f, -4.0f),
        vec3(0.0f, 1.0f, 0.0f),
        0.1f, 100000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    /* ===================================================================================
                Cornell Box
    =====================================================================================*/

    /** /
    sref<Model> CB = make_sref<Model>("CB");
    CB->loadFromMemory(getAssetSource("models/CornellBox/CornellBox-Original.obj"), getAssetSource("models/CornellBox/CornellBox-Original.mtl"));
    RM.addModel("CB", CB);
    CB->prepare();
    CB->setScale(3.0f, 3.0f, 3.0f);
    glApp->getScene()->addModel(CB);

    sref<Camera> camera = make_sref<Perspective>(width, height,
        vec3(0.0f, 3.0f, 8.5f),
        vec3(0.0f, 3.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        0.1f, 1000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    /* ===================================================================================
                Sibenik
    =====================================================================================*/

    /** /
    loadTextures("models/Sibenik Modified", "png", "");
    checkOpenGLError("Error during texture loading!");
    LOG("Textures loaded...\n");

    sref<Model> sibenik = make_sref<Model>("sibenik");
    sibenik->loadFromMemory(getAssetSource("models/Sibenik Modified/sibenik.obj"), getAssetSource("models/Sibenik Modified/sibenik.mtl"));
    RM.addModel("sibenik", sibenik);
    sibenik->prepare();
    //sibenik->setScale(0.05f, 0.05f, 0.05f);
    glApp->getScene()->addModel(sibenik);

    sref<Camera> camera = make_sref<Perspective>(width, height,
        vec3(-2.0f, 5.0f, 1.5f),
        vec3(13.0f, 2.0f, -1.0f),
        vec3(0.0f, 1.0f, 0.0f),
        0.1f, 100000.0f, 60.0f);

    glApp->setCamera(camera);
    glApp->getScene()->addCamera(camera);
    /**/

    checkOpenGLError("Error during model loading!");
    LOG("Meshes and models loaded...\n");
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
    float dt = float(deltaTime) / 1000.0f;

    float secs = dt / 1000000.0f;
    LOG("FPS: %f\n", 1.0f / secs);

    totalFrames++;
    totalTimePerFrame += secs ;

    LOG("AVG FPS: %f\n", 1.0f / (totalTimePerFrame / (float) totalFrames));

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

