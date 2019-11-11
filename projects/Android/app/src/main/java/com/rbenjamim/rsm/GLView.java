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

    private MainActivity mainActivity;

    public GLView(Context context, AssetManager aMgr){
        super(context);
        assetManager = aMgr;

        setEGLConfigChooser(true);
        setEGLConfigChooser(RED_BITS, GREEN_BITS, BLUE_BITS, ALPHA_BITS, DEPTH_BITS, STENCIL_BITS);
        //setEGLConfigChooser(new ConfigChooser());
        setEGLContextClientVersion(EGL_VERSION);
        setRenderer(new Renderer());

        mainActivity = (MainActivity) context;
    }

    private class Renderer implements GLSurfaceView.Renderer{

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
            mainActivity.setFPS(Math.min(AndroidApp.getInstantFPS(), 60));
        }
    }

    protected class ConfigChooser implements GLSurfaceView.EGLConfigChooser
    {
        public EGLConfig chooseConfig(EGL10 egl, EGLDisplay display)
        {
            int configAttributes[] = {EGL10.EGL_RED_SIZE, RED_BITS,
                    EGL10.EGL_GREEN_SIZE, GREEN_BITS,
                    EGL10.EGL_BLUE_SIZE, BLUE_BITS,
                    EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                    EGL10.EGL_SAMPLES, SAMPLE_BITS,
                    EGL10.EGL_DEPTH_SIZE, DEPTH_BITS,
                    EGL10.EGL_STENCIL_SIZE, STENCIL_BITS,
                    EGL10.EGL_NONE};
            int num_config[] = new int[1];

            egl.eglChooseConfig(display, configAttributes, null, 0, num_config);

            int       numConfigs = num_config[0];
            EGLConfig configs[]  = new EGLConfig[numConfigs];

            egl.eglChooseConfig(display, configAttributes, configs, numConfigs, num_config);

            return configs[0];
        }
    }
}
