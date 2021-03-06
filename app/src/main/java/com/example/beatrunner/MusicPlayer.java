package com.example.beatrunner;

import androidx.appcompat.app.AppCompatActivity;

import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MusicPlayer extends AppCompatActivity {

    public static String TAG = "MusicPlayer";
    public static final int MUSIC_ID = R.raw.coldplay_kygo;

    private TextView debug;
    private Button buttonPlay;
    private Button buttonPause;
    private Button buttonStop;

    private MediaPlayerFunctionalities player;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_music_player);
        initializeUI();
        initializePlayer();
    }

    @Override
    protected void onStart(){
        super.onStart();
        player.load(MUSIC_ID);
    }

    private void initializeUI() {
        debug = findViewById(R.id.debugTextView);
        buttonPlay = findViewById(R.id.button_play);
        buttonPause = findViewById(R.id.button_pause);
        buttonStop = findViewById(R.id.button_pause);
    }

    private void initializePlayer() {
        MediaPlayerWrapper mediaPlayerWrapper = new MediaPlayerWrapper(this);
        player = mediaPlayerWrapper;
        player.initializePlayer();
    }

    public void playMusic(View view) {
        player.play();
    }

    public void pauseMusic(View view) {
        player.pause();
    }

    public void stopMusic(View view) {
        player.stop();
    }
}
