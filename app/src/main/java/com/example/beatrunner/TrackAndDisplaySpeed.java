package com.example.beatrunner;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import org.w3c.dom.Text;

public class TrackAndDisplaySpeed extends AppCompatActivity implements SensorEventListener {

    TextView stepsDisplay;
    Button resumeOrPause;

    SensorManager sensorManager;
    Vibrator vibrator;

    int steps = 0;

    private String RESET = "Reset";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_track_and_display_speed);

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        stepsDisplay = (TextView) findViewById(R.id.stepDisplay);
        resumeOrPause = (Button) findViewById(R.id.stopCounting);
    }

    protected void onResume() {
        super.onResume();
        Sensor sensor = sensorManager.getDefaultSensor(Sensor.TYPE_STEP_DETECTOR);
        if (sensor != null) {
            sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_FASTEST);
        } else {
            Toast.makeText(this, "Sensor not found!", Toast.LENGTH_SHORT).show();
        }
    }

    protected void onPause() {
        super.onPause();
    }

    @Override
    public void onSensorChanged(SensorEvent event) {
//        VibrationEffect vibrationEffect = VibrationEffect.createOneShot(50, VibrationEffect.DEFAULT_AMPLITUDE);
        vibrator.vibrate(50);
        steps ++;
        stepsDisplay.setText(Integer.toString(steps));
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }
}
