package com.example.beatrunner;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.media.AudioManager;
import android.os.Bundle;
import android.widget.Toast;

public class SuperPlayer extends AppCompatActivity {

    private boolean hasAllPermissions = false;
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
            if (sampleRateString == null) sampleRateString = "48000";
            if (buffersizeString == null) buffersizeString = "4800";
        }
        int sampleRate = Integer.parseInt(sampleRateString);
        int bufferSize = Integer.parseInt(buffersizeString);
        System.loadLibrary("BeatRunner");
        BeatRunner(sampleRate, bufferSize);
    }
    private native void BeatRunner (int sampleRate, int bufferSize);
}
