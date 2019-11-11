 package com.rbenjamim.rsm;

import android.app.Activity;
import android.content.pm.ActivityInfo;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v4.content.ContextCompat;
import android.view.Window;
import android.widget.FrameLayout;
import android.widget.TextView;

 public class MainActivity extends Activity {

    private GLView glView;
    private AssetManager assetManager;

    private TextView FPSCounter;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);

        this.setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        this.requestWindowFeature(Window.FEATURE_NO_TITLE);

        assetManager = getResources().getAssets();
        glView = new GLView(this, assetManager);

        int uiOptions = GLView.SYSTEM_UI_FLAG_HIDE_NAVIGATION | GLView.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION  | GLView.SYSTEM_UI_FLAG_FULLSCREEN;
        glView.setSystemUiVisibility(uiOptions);


        // FPS Counter (Upper Right Corner)
        FrameLayout frameLayout = new FrameLayout(this);
        frameLayout.addView(glView);

        FPSCounter = new TextView(this);
        FPSCounter.setTextColor(ContextCompat.getColor(this, android.R.color.white));
        FPSCounter.setPadding(50, 50, 0, 0);

        frameLayout.addView(FPSCounter);
        setContentView(frameLayout);
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

    public void setFPS(final int fps){
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                FPSCounter.setText(fps + "FPS");
            }
        });
    }

}