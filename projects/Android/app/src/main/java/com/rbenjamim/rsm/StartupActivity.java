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

    private RadioButton radForward;
    private RadioButton radDeferred;
    private RadioButton radAccelerated;

    private Button btStart;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.startuplayout);

        radSNC = findViewById(R.id.radSNC);
        radLucy = findViewById(R.id.radLucy);
        radSponza = findViewById(R.id.radSponza);

        radHD = findViewById(R.id.radHD);
        radFHD = findViewById(R.id.radFHD);

        radFullRes = findViewById(R.id.radFullRes);
        radHalfRes = findViewById(R.id.radHalfRes);

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
        startActivity(intent);
    }

}
