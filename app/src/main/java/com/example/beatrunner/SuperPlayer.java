package com.example.beatrunner;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.pm.PackageManager;
import android.content.res.AssetFileDescriptor;
import android.media.AudioManager;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.Toast;

import java.io.IOException;

public class SuperPlayer extends AppCompatActivity {

    private boolean hasAllPermissions = false;
    private boolean isPlaying = false;
    AudioManager audioManager;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_super_player);
        getPermissions();
        if (hasAllPermissions) {
            initialize();
        }
    }

    private void getPermissions() {
        String[] permissions = { Manifest.permission.MODIFY_AUDIO_SETTINGS };
        for (String permission:permissions) {
            if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(this, permissions, 0);
                return;
            }
        }
        hasAllPermissions = true;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, @NonNull String[] permissions, @NonNull int[] grantResults) {
        if ((requestCode != 0 || grantResults.length < 1 || grantResults.length != permissions.length)) {
            return;
        }
        hasAllPermissions = true;
        for (int grantResult:grantResults) {
            if (grantResult != PackageManager.PERMISSION_GRANTED) {
                hasAllPermissions = false;
                Toast.makeText(getApplicationContext(), "Please allow!", Toast.LENGTH_LONG).show();
            }
        }
    }

    private void initialize() {
        String sampleRateString = null;
        String buffersizeString = null;
        audioManager = (AudioManager) this.getSystemService(Context.AUDIO_SERVICE);
        if (audioManager != null) {
            sampleRateString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_SAMPLE_RATE);
            buffersizeString = audioManager.getProperty(AudioManager.PROPERTY_OUTPUT_FRAMES_PER_BUFFER);
        }
        if (sampleRateString == null) sampleRateString = "48000";
        if (buffersizeString == null) buffersizeString = "480";
        int sampleRate = Integer.parseInt(sampleRateString);
        int bufferSize = Integer.parseInt(buffersizeString);

        String path = getPackageResourcePath();
        AssetFileDescriptor fd = getResources().openRawResourceFd(R.raw.brother);
        int fileOffset = (int)fd.getStartOffset();
        int fileLength = (int)fd.getLength();
        try {
            fd.getParcelFileDescriptor().close();
        } catch (IOException e) {
            Log.e("SuperPlayer", "Close error.");
        }

        System.loadLibrary("BeatRunner");
        BeatRunnerInit(sampleRate, bufferSize);
        OpenFile(path, fileOffset, fileLength);
        // TimeStretch(path, fileOffset, fileLength);
    }

    public void ButtonTogglePlayPause(View view) {
        TogglePlayPause();
        Button button = findViewById(R.id.button);
        if (isPlaying) {
            button.setText("Play");
        } else {
            button.setText("Pause");
        }
        isPlaying = !isPlaying;
    }

    private native void BeatRunnerInit (int sampleRate, int bufferSize);
    private native void OpenFile(String path, int offset, int length);
    private native void TimeStretch(String path, int offset, int length);
    private native void TogglePlayPause();
}