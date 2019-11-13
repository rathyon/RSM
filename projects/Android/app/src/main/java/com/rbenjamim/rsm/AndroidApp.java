package com.rbenjamim.rsm;

import android.content.res.AssetManager;

public class AndroidApp {
    static {
        System.loadLibrary("AndroidRSM-lib");
    }

    public static native void init(AssetManager assetManager, int scene, int rsmVersion, int numVPL, int indirectWidth, int indirectHeight, int gbufferResolution, int rsmBufferResolution);
    public static native void resize(int width, int height);
    public static native void render();
    public static native int getInstantFPS();
}
