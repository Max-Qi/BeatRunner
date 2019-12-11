package com.example.beatrunner;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

public class MainActivity extends AppCompatActivity {

    Button startCadenceTracker;
    Button startMusicPlayer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        startCadenceTracker = (Button) findViewById(R.id.buttonStartCadence);
        startMusicPlayer = (Button) findViewById(R.id.buttonStartMusic);
    }

    public void trackSpeed(View view) {
        Intent intent = new Intent(this, TrackAndDisplaySpeed.class);
        startActivity(intent);
    }

    public void startGroovin(View view) {
        Intent intent = new Intent(this, MusicPlayer.class);
        startActivity(intent);
    }

    private native void  onFxOff();
}
