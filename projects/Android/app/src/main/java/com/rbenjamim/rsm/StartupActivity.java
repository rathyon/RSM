package com.rbenjamim.rsm;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.RadioButton;

public class StartupActivity extends Activity {

    private RadioButton radSNC;
    private RadioButton radLucy;
    private RadioButton radSponza;

    private RadioButton radHD;
    private RadioButton radFHD;

    private RadioButton radFullRes;
    private RadioButton radHalfRes;

    private RadioButton rad32, rad64, rad128, rad256;
    private RadioButton rad32i, rad64i, rad128i, rad256i;

    private RadioButton radForward;
    private RadioButton radDeferred;
    private RadioButton radAccelerated;

    private Button btStart;

    private int scene, rsmVersion, numVPL, indirectWidth, indirectHeight, gbufferResolution, rsmResolution;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.startuplayout);

        // set default values, just in case...
        scene = 0;
        rsmVersion = 0;
        numVPL = 32;
        indirectWidth = 32;
        indirectHeight = 32;
        gbufferResolution = 0;
        rsmResolution = 1;


        radSNC = findViewById(R.id.radSNC);
        radLucy = findViewById(R.id.radLucy);
        radSponza = findViewById(R.id.radSponza);

        radHD = findViewById(R.id.radHD);
        radFHD = findViewById(R.id.radFHD);

        radFullRes = findViewById(R.id.radFullRes);
        radHalfRes = findViewById(R.id.radHalfRes);

        rad32i = findViewById(R.id.rad32i);
        rad64i = findViewById(R.id.rad64i);
        rad128i = findViewById(R.id.rad128i);
        rad256i = findViewById(R.id.rad256i);

        rad32 = findViewById(R.id.rad32);
        rad64 = findViewById(R.id.rad64);
        rad128 = findViewById(R.id.rad128);
        rad256 = findViewById(R.id.rad256);

        radForward = findViewById(R.id.radForward);
        radDeferred = findViewById(R.id.radDeferred);
        radAccelerated = findViewById(R.id.radAccelerated);

        btStart = findViewById(R.id.btStart);
    }

    public void onStartButtonClick(View view){
        Intent intent = new Intent(this, MainActivity.class);

        intent.putExtra("scene", scene);
        intent.putExtra("rsmVersion", rsmVersion);
        intent.putExtra("numVPL", numVPL);
        intent.putExtra("indirectWidth", indirectWidth);
        intent.putExtra("indirectHeight",indirectHeight);
        intent.putExtra("gbufferResolution", gbufferResolution);
        intent.putExtra("rsmResolution", rsmResolution);

        startActivity(intent);
    }

    public void onSceneClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        // Check which radio button was clicked
        switch(view.getId()) {
            case R.id.radSNC:
                if (checked)
                    scene = 0;
                    break;
            case R.id.radLucy:
                if (checked)
                    scene = 1;
                    break;
            case R.id.radSponza:
                if (checked)
                    scene = 2;
                    break;
        }
    }

    public void onResolutionClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        // Check which radio button was clicked
        switch(view.getId()) {
            case R.id.radHD:
                if (checked)
                    gbufferResolution = 0;
                break;
            case R.id.radFHD:
                if (checked)
                    gbufferResolution = 1;
                break;
        }
    }

    public void onRSMScaleClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        // Check which radio button was clicked
        switch(view.getId()) {
            case R.id.radFullRes:
                if (checked)
                    rsmVersion = 1;
                break;
            case R.id.radHalfRes:
                if (checked)
                    rsmVersion = 2;
                break;
        }
    }

    public void onIndirectClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        switch(view.getId()) {
            case R.id.rad32i:
                if (checked){
                    indirectWidth = 32;
                    indirectHeight = 32;
                }
                break;
            case R.id.rad64i:
                if (checked){
                    indirectWidth = 64;
                    indirectHeight = 64;
                }
                break;
            case R.id.rad128i:
                if (checked){
                    indirectWidth = 128;
                    indirectHeight = 128;
                }
                break;
            case R.id.rad256i:
                if (checked){
                    indirectWidth = 256;
                    indirectHeight = 256;
                }
                break;
        }
    }

    public void onSamplesClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        switch(view.getId()) {
            case R.id.rad32:
                if (checked){
                    numVPL = 32;
                }
                break;
            case R.id.rad64:
                if (checked){
                    numVPL = 64;
                }
                break;
            case R.id.rad128:
                if (checked){
                    numVPL = 128;
                }
                break;
            case R.id.rad256:
                if (checked){
                    numVPL = 256;
                }
                break;
        }
    }

    public void onVersionClick(View view) {
        boolean checked = ((RadioButton) view).isChecked();

        // Check which radio button was clicked
        switch(view.getId()) {
            case R.id.radForward:
                if (checked)
                    rsmVersion = 0;
                break;
            case R.id.radDeferred:
                if (checked)
                    rsmVersion = 1;
                break;
            case R.id.radAccelerated:
                if (checked)
                    rsmVersion = 2;
                break;
        }
    }

}
