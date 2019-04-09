package com.rbenjamim.rsm;

import android.content.res.AssetManager;

public class AndroidApp {
    static {
        System.loadLibrary("AndroidRSM-lib");
    }

    public static native void init(AssetManager assetManager);
    public static native void resize(int width, int height);
    public static native void render();

}
