 package com.rbenjamim.rsm;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.view.Window;

 public class MainActivity extends Activity {

    private GLView glView;
    private AssetManager assetManager;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);

        assetManager = getResources().getAssets();
        glView = new GLView(this, assetManager);

        int uiOptions = GLView.SYSTEM_UI_FLAG_HIDE_NAVIGATION | GLView.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION  | GLView.SYSTEM_UI_FLAG_FULLSCREEN;
        glView.setSystemUiVisibility(uiOptions);

        setContentView(glView);
    }

    @Override
    protected void onPause() {
        super.onPause();
        glView.onPause();
    }

    @Override
    protected void onResume() {
        super.onResume();
        glView.onResume();
    }

}