package com.rbenjamim.gir;

public class MobileApp {
    static {
        System.loadLibrary("MobileGIRAppLib");
    }

    public static native void init();
    public static native void resize(int width, int height);
    public static native void render();

}
