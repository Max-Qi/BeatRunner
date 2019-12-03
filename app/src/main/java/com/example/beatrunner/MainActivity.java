package com.example.beatrunner;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
    }

    public void trackSpeed(View view) {
        Intent intent = new Intent(this, TrackAndDisplaySpeed.class);
        startActivity(intent);
    }
}
