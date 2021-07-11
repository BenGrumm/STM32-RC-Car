package com.example.bluetoothrccontroller;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.appcompat.app.AlertDialog;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.fragment.app.DialogFragment;

import android.Manifest;
import android.app.Dialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;

import java.io.IOException;
import java.io.OutputStream;
import java.nio.charset.Charset;
import java.util.Arrays;
import java.util.Set;
import java.util.UUID;

public class MainActivity extends AppCompatActivity {

    private static final String TAG = MainActivity.class.getSimpleName();
    private static final int REQUEST_ENABLE_BT = 12;
    private static final int PERMISSION_CODE = 85;

    private TextView directionTextView;
    private ImageButton leftButton, rightButton, forwardButton, backButton, stopButton;

    private MyBluetoothService mbs;
    private Handler btHandler;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        leftButton = findViewById(R.id.leftButton);
        rightButton = findViewById(R.id.rightButton);
        forwardButton = findViewById(R.id.upButton);
        backButton = findViewById(R.id.downButton);
        stopButton = findViewById(R.id.stopButton);
        directionTextView = findViewById(R.id.infoTextView);

        checkBTPermissions();

        Handler.Callback callback = new Handler.Callback() {
            @Override
            public boolean handleMessage(@NonNull Message message) {

                switch(message.what) {
                    case MyBluetoothService.MessageConstants.MESSAGE_READ:
                        Log.d(TAG, "Handler Called arg1 = " + message.arg1);

                        if (message.arg1 != -1) {
                            byte[] readBuf = (byte[]) message.obj;
                            String buffString = new String(readBuf, 0, message.arg1);

                            if(buffString.equals("!")){
                                directionTextView.setText("Stopped");
                                AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
                                builder.setTitle("Obstacle Detected").setMessage("An Obstacle Was Detected In The Path Of The Vehicle");
                                builder.setNeutralButton("Ok", new DialogInterface.OnClickListener() {
                                    @Override
                                    public void onClick(DialogInterface dialogInterface, int i) {
                                        dialogInterface.cancel();
                                    }
                                });

                                builder.create().show();
                            }

                            Log.e(TAG, buffString);
                        }
                        break;
                    case MyBluetoothService.MessageConstants.CONNECT_ERROR:
                        Toast.makeText(MainActivity.this, "Unable to connect (handler)", Toast.LENGTH_SHORT).show();
                        break;
                    case MyBluetoothService.MessageConstants.MESSAGE_TOAST:
                        if(message.obj != null){
                            Toast.makeText(MainActivity.this, (String)message.obj, Toast.LENGTH_SHORT).show();
                        }
                        break;
                }
                return false;
            }
        };

        btHandler = new Handler(Looper.getMainLooper(), callback);

        try {
            // mbs = new MyBluetoothService(btHandler);
        }catch (Exception e){
            Log.d(TAG, "Error Connecting To BT, " + e.getMessage());
        }

        leftButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mbs != null && mbs.isConnected()){
                    mbs.writeString("L");
                    directionTextView.setText("Left");
                } else{
                    Toast.makeText(MainActivity.this, "Not Connected", Toast.LENGTH_SHORT).show();
                }
            }
        });

        rightButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mbs != null && mbs.isConnected()){
                    mbs.writeString("R");
                    directionTextView.setText("Right");
                }else{
                    Toast.makeText(MainActivity.this, "Not Connected", Toast.LENGTH_SHORT).show();
                }
            }
        });

        forwardButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mbs != null && mbs.isConnected()){
                    mbs.writeString("F");
                    directionTextView.setText("Forwards");
                }else{
                    Toast.makeText(MainActivity.this, "Not Connected", Toast.LENGTH_SHORT).show();
                }
            }
        });

        backButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mbs != null && mbs.isConnected()){
                    mbs.writeString("B");
                    directionTextView.setText("Backwards");
                }else{
                    Toast.makeText(MainActivity.this, "Not Connected", Toast.LENGTH_SHORT).show();
                }
            }
        });

        stopButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if(mbs != null && mbs.isConnected()){
                    mbs.writeString("S");
                    directionTextView.setText("Stopped");
                }else{
                    Toast.makeText(MainActivity.this, "Not Connected", Toast.LENGTH_SHORT).show();
                }
            }
        });
    }

    private void checkBTPermissions(){
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q) {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_BACKGROUND_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                ActivityCompat.requestPermissions(MainActivity.this, new String[]{Manifest.permission.ACCESS_BACKGROUND_LOCATION}, PERMISSION_CODE);
            }
        }
    }

    public void getBtDeviceToConnect(){
        final BluetoothAdapter bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();

        if(bluetoothAdapter.isEnabled()){
            Set<BluetoothDevice> devices = bluetoothAdapter.getBondedDevices();

            final BluetoothDevice[] devicesArray = Arrays.copyOf(devices.toArray(), devices.size(), BluetoothDevice[].class);
            String[] deviceNames = new String[devicesArray.length];

            for(int i = 0; i < deviceNames.length; i++){
                deviceNames[i] = devicesArray[i].getName();
            }

            AlertDialog.Builder builder = new AlertDialog.Builder(MainActivity.this);
            builder.setTitle("Pick Device To Connect")
                    .setItems(deviceNames, new DialogInterface.OnClickListener() {
                        @Override
                        public void onClick(DialogInterface dialogInterface, int i) {
                            Log.e(TAG, "Position = " + i);
                            try {
                                if(mbs != null){
                                    mbs.close();
                                }
                                mbs = new MyBluetoothService(btHandler, bluetoothAdapter, devicesArray[i]);
                            }catch (Exception e){
                                Toast.makeText(MainActivity.this, "Unable to connect", Toast.LENGTH_SHORT).show();
                            }

                        }
                    });

            AlertDialog ad = builder.create();
            ad.show();
        }
    }

    public boolean isConnected(){
        return mbs != null && mbs.isConnected();
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.bt_connect_menu, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(@NonNull MenuItem item) {
        switch (item.getItemId()){
            case R.id.bt_connect:
                Log.d(TAG, "Connect Here");
                getBtDeviceToConnect();
                return true;
            default:
                return super.onOptionsItemSelected(item);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, @Nullable Intent data) {
        switch (requestCode) {
            case REQUEST_ENABLE_BT:
                if(resultCode == RESULT_OK) {
                    Toast.makeText(this, "Bluetooth Switched On", Toast.LENGTH_SHORT).show();
                }
                break;
        }
        super.onActivityResult(requestCode, resultCode, data);
    }
}