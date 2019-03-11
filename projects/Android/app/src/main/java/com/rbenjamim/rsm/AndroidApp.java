package com.rbenjamim.rsm;

public class AndroidApp {
    static {
        System.loadLibrary("AndroidRSM-lib");
    }

    public static native void init();
    public static native void resize(int width, int height);
    public static native void render();

}
