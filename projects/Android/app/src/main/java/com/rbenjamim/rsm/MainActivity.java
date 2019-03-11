/**/
 package com.rbenjamim.rsm;

import android.app.Activity;
import android.os.Bundle;

public class MainActivity extends Activity {

    private GLView glView;

    @Override
    protected void onCreate(Bundle savedInstanceState){
        super.onCreate(savedInstanceState);
        glView = new GLView(this);
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
/**/