package com.example.beatrunner;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Context;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.os.Bundle;
import android.os.Vibrator;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

public class TrackAndDisplaySpeed extends AppCompatActivity implements SensorEventListener {

    static int INTERVAL = 5000;

    TextView stepsDisplay;
    TextView cadenceDisplay;
    Button resumeOrPause;
    SensorManager sensorManager;
    Vibrator vibrator;

    long startTimeMS;
    List<Long> stepTimesMS = new ArrayList<Long>();
    float cadence = -1;
    boolean running;

    int steps = 0;

    private String START = "Start";
    private String STOP = "Stop";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_track_and_display_speed);

        sensorManager = (SensorManager) getSystemService(Context.SENSOR_SERVICE);
        vibrator = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        stepsDisplay = (TextView) findViewById(R.id.stepDisplay);
        cadenceDisplay = (TextView) findViewById(R.id.cadenceDisplay);
        resumeOrPause = (Button) findViewById(R.id.changeButton);
        startTimeMS = System.currentTimeMillis();

        running = true;
        resumeOrPause.setText(STOP);
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
        if (running) {
            long currentTimeMS = System.currentTimeMillis();
            vibrator.vibrate(30);
            steps++;
            stepsDisplay.setText(Integer.toString(steps));
            stepTimesMS.add(System.currentTimeMillis());
            if (currentTimeMS - startTimeMS >= INTERVAL) {
                cadence = calculateCadence(currentTimeMS);
            } else {
                cadence = -1;
            }
            if (cadence > 0) {
                cadenceDisplay.setText(Float.toString(cadence));
            } else {
                cadenceDisplay.setText(null);
            }
        }
    }

    @Override
    public void onAccuracyChanged(Sensor sensor, int accuracy) {

    }

    private float calculateCadence(long currentTimeMS) {
        while (currentTimeMS - stepTimesMS.get(0) >= INTERVAL) {
            stepTimesMS.remove(0);
        }
        return (float) (60.0 / ((currentTimeMS - stepTimesMS.get(0)) / 1000.0) * (float) stepTimesMS.size());
    }

    public void change (View view) {
        if (running) {
            running = false;
            resumeOrPause.setText(START);
            stepTimesMS.clear();
        } else {
            startTimeMS = System.currentTimeMillis();
            running = true;
            resumeOrPause.setText(STOP);
        }
    }

}
