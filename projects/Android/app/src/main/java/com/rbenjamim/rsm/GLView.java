package com.rbenjamim.rsm;

import android.content.Context;
import android.content.res.AssetManager;
import android.opengl.GLES32;
import android.opengl.GLSurfaceView;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.opengles.GL10;

import static com.rbenjamim.rsm.Configs.*;

public class GLView extends GLSurfaceView {

    public static AssetManager assetManager;

    public GLView(Context context, AssetManager aMgr){
        super(context);
        assetManager = aMgr;

        setEGLConfigChooser(RED_BITS, GREEN_BITS, BLUE_BITS, ALPHA_BITS, DEPTH_BITS, STENCIL_BITS);
        //setEGLConfigChooser(new ConfigChooser());
        setEGLContextClientVersion(EGL_VERSION);
        setRenderer(new Renderer());
    }

    private static class Renderer implements GLSurfaceView.Renderer{

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            AndroidApp.init(assetManager);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            AndroidApp.resize(width, height);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            AndroidApp.render();
        }
    }


    // TODO Implement Config Chooser for OpenGL ES settings
/** /
    private static class ConfigChooser implements  GLSurfaceView.EGLConfigChooser {

        @Override
        public EGLConfig chooseConfig(EGL10 gl, EGLDisplay display) {

        }
    }
    /**/
}
