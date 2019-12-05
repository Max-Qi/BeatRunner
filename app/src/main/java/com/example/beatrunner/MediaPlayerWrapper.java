package com.example.beatrunner;

import android.content.Context;
import android.content.res.AssetFileDescriptor;
import android.media.MediaPlayer;

import java.io.IOException;

public class MediaPlayerWrapper implements MediaPlayerFunctionalities {

    private MediaPlayer mediaPlayer;
    private final Context mainContext;

    public MediaPlayerWrapper(Context context) {
        mainContext = context;
    }


    @Override
    public void initializePlayer() {
        if (mediaPlayer == null) {
            mediaPlayer = new MediaPlayer();
            mediaPlayer.setOnCompletionListener(new MediaPlayer.OnCompletionListener() {
                @Override
                public void onCompletion(MediaPlayer mp) {
                    stop();
                }
            });
        }
    }

    @Override
    public void load(int resourceID) {
        AssetFileDescriptor assetFileDescriptor = mainContext.getResources().openRawResourceFd(resourceID);
        try {
            mediaPlayer.setDataSource(assetFileDescriptor);
        } catch (IOException e) {
            e.printStackTrace();
        }

        try {
            mediaPlayer.prepare();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    @Override
    public void play() {
        if (!mediaPlayer.isPlaying()) {
            mediaPlayer.start();
        }
    }

    @Override
    public void pause() {
        mediaPlayer.pause();
    }

    @Override
    public void stop() {
        mediaPlayer.seekTo(0);
        mediaPlayer.pause();
    }
}
