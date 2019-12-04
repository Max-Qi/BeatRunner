package com.example.beatrunner;

import android.media.MediaPlayer;

public class MediaPlayerWrapper implements MediaPlayerFunctionalities {

    private MediaPlayer mediaPlayer;

    public void initializeMediaPlayer() {
        mediaPlayer = new MediaPlayer();
        mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
            @Override
            public void onCompletion(MediaPlayer mp) {
                
            }
        });
    }



    @Override
    public void load(int resourceID) {

    }

    @Override
    public void play() {

    }

    @Override
    public void pause() {

    }

    @Override
    public void stop() {

    }
}
