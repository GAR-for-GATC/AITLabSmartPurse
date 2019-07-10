
/*
I couldnt figure out how to import a library, so the ToastCompat library will
also be placed into this project.  The ToastCompat library files are named:
ToastCompat.java
BadTokenListener.java
SafeToastContext.java
This library is needed because the phone that's being tested runs andriod version 7.1, which
causes issues with Toast.
https://stackoverflow.com/questions/51532449/fatal-exception-android-view-windowmanagerbadtokenexception-unable-to-add-wind#
Edit: This wasn't the issue, but i haven't gotten around to getting rid of the code yet.

pseudocode for bluetooth functionality:
on app resume:
If bt is off on the phone, then ask the user to turn on bluetooth.
if user says no, then don't initialize bluetooth.
if user says yes, then initialize bluetooth.
attempt to connect to bluetooth module.
if couldn't connect, then output: "could not connect to purse"
if connection occurs then output "purse connected"

on app close:
close connection to purse

when bluetooth button is pressed in off configuration:
ask user to turn on bluetooth.
if user says no, then don't initialize bluetooth.
if user says yes, then initialize bluetooth.
attempt to connect to bluetooth module.
if couldn't connect, then output: "could not connect to purse"
if connection occurs then output "purse connected"

when bluetooth button is in on configuration:
if connected to purse, then disconnect
then turn off bluetooth, output, "bluetooth disabled"
===============
todo: need add to a "Connect to Purse" button, and add a case if the purse is disconnected while the app is open.
todo: recieve data from arduino
todo: preview colour data in app.

*/



package com.example.testapp;

import androidx.appcompat.app.AppCompatActivity;

import android.content.Intent;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.app.Activity;
import android.preference.PreferenceManager;
import android.text.Editable;
import android.text.TextWatcher;
import android.widget.EditText;
import android.widget.SeekBar;
import android.util.Log;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.ParcelUuid;
import android.bluetooth.BluetoothSocket;
import java.io.OutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.Arrays;

import java.util.Set;
import android.widget.Toast;

import android.view.View;
import java.util.ArrayList;
import android.widget.ArrayAdapter;
import android.widget.ListView;

import android.widget.ToggleButton;
import android.widget.Button;
import android.widget.CompoundButton;

import android.view.MotionEvent;
import android.graphics.Rect;
import android.view.inputmethod.InputMethodManager;
import 	android.content.Context;
import android.view.WindowManager;
import android.view.View.OnFocusChangeListener;



public class MainActivity extends AppCompatActivity {
    public EditText redText;
    private SeekBar redBar;
    public EditText greenText;
    private SeekBar greenBar;
    public EditText blueText;
    private SeekBar blueBar;
    public EditText brightText;
    private SeekBar brightBar;

    private int savedProgressRed;
    private int loadProgressRed;
    private int savedProgressGreen;
    private int loadProgressGreen;
    private int savedProgressBlue;
    private int loadProgressBlue;
    private int savedProgressBright;
    private int loadProgressBright;
    private static final String RED_PROGRESS = "redBar_progress";
    private static final String GREEN_PROGRESS = "greenBar_progress";
    private static final String BLUE_PROGRESS = "blueBar_progress";
    private static final String BRIGHT_PROGRESS = "brightBar_progress";

    private BluetoothAdapter myBA;
    private OutputStream btOutputStream;
    private InputStream btInStream;


    SharedPreferences myPreferences;
    SharedPreferences.Editor editor;

    private ToggleButton btTB;
    private Button onOff;
    private int noBT = 0; //if 1, then there's no bluetooth on the phone.

    final Context ctx = this;
    BluetoothSocket btSocket = null;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        //Red Slider/////////////////////////////////////
        redBar = findViewById(R.id.redSeekBar);
        redBar.setOnSeekBarChangeListener(new redListener());
        redText = findViewById(R.id.editRedText);
        redText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {}
            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                //redBar.setProgress(Integer.parseInt(charSequence.toString()));
            }
            @Override
            public void afterTextChanged(Editable editable){
                if(isEmpty(redText) == false){
                    //intFat = Integer.parseInt(etFat.getText().toString());
                    redBar.setProgress(Integer.parseInt(redText.getText().toString()));
                }
                else{
                    redText.setText("0");
                    redText.hasFocus();
                }
                redText.setSelection(redText.getText().length());
                if(Integer.parseInt(redText.getText().toString()) > 255){
                    redText.setText("255");
                }

            }
        });
        //send data after user has unfocused from the textEdit
        redText.setOnFocusChangeListener(new OnFocusChangeListener() {
            public void onFocusChange(View v, boolean hasFocus) {
                if(!hasFocus) {
                    if(myBA != null){
                        if(myBA.isEnabled()){
                            try{
                                btOutputStream.write(0b00000001);
                                //change bytes of chars in redText to a single byte to make
                                //parsing of the text easier on the purse side.
                                byte[] redOutput = redText.getText().toString().getBytes();
                                String redTransmited = "";
                                for(int i=0; i<redOutput.length;i++){
                                    redTransmited += Integer.toString((int)redOutput[i]-48); //"-48" changes ascii into regular numbers.
                                }
                                //since bytes in java are signed and do not go up to 255
                                //the hardware multiplies output1 by 2 and then adds output2
                                //to get the new setting for the red value.
                                byte redoutput1 = (byte)(Integer.parseInt(redTransmited)/2);
                                byte redoutput2 = (byte)(Integer.parseInt(redTransmited)%2);
                                Log.d("kitty", ""+redoutput1);
                                Log.d("kitty", ""+redoutput2);
                                btOutputStream.write(redoutput1);
                                btOutputStream.write(redoutput2);
                                btOutputStream.write(0b00000000);

                            }catch(IOException e){
                                Log.e("error!", ""+e);
                            }
                        }
                    }
                }
            }
        });

        //Green Slider///////////////////////////////////
        greenBar = findViewById(R.id.greenSeekBar);
        greenBar.setOnSeekBarChangeListener(new greenListener());
        greenText = findViewById(R.id.editGreenText);
        greenText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {}
            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                //greenBar.setProgress(Integer.parseInt(charSequence.toString()));
            }
            @Override
            public void afterTextChanged(Editable editable){
                if(isEmpty(greenText) == false){
                    //intFat = Integer.parseInt(etFat.getText().toString());
                    greenBar.setProgress(Integer.parseInt(greenText.getText().toString()));
                }
                else{
                    greenText.setText("0");
                    greenText.hasFocus();
                }
                greenText.setSelection(greenText.getText().length());
                if(Integer.parseInt(greenText.getText().toString()) > 255){
                    greenText.setText("255");
                }

                /*
                try{
                    btOutputStream.write(0b00000010);
                    btOutputStream.write(greenText.getText().toString().getBytes());
                }catch(Exception e){

                }*/
            }
        });
        //send data after user has unfocused from the textEdit
        greenText.setOnFocusChangeListener(new OnFocusChangeListener() {
            public void onFocusChange(View v, boolean hasFocus) {
                if(!hasFocus) {
                    if(myBA != null){
                        if(myBA.isEnabled()){
                            try{
                                btOutputStream.write(0b00000010);
                                //change bytes of chars in redText to a single byte to make
                                //parsing of the text easier on the purse side.
                                byte[] greenOutput = greenText.getText().toString().getBytes();
                                String greenTransmited = "";
                                for(int i=0; i<greenOutput.length;i++){
                                    greenTransmited += Integer.toString((int)greenOutput[i]-48); //"-48" changes ascii into regular numbers.
                                }
                                //since bytes in java are signed and do not go up to 255
                                //the hardware multiplies output1 by 2 and then adds output2
                                //to get the new setting for the red value.
                                byte greenoutput1 = (byte)(Integer.parseInt(greenTransmited)/2);
                                byte greenoutput2 = (byte)(Integer.parseInt(greenTransmited)%2);
                                btOutputStream.write(greenoutput1);
                                btOutputStream.write(greenoutput2);
                                btOutputStream.write(0b00000000);

                            }catch(IOException e){
                                Log.e("error!", ""+e);
                            }
                        }
                    }
                }
            }
        });

        //Blue Slider////////////////////////////////////
        blueBar = findViewById(R.id.blueSeekBar);
        blueBar.setOnSeekBarChangeListener(new blueListener());
        blueText = findViewById(R.id.editBlueText);
        blueText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {

            }
            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                //blueBar.setProgress(Integer.parseInt(blueText.getText().toString()));
            }
            @Override
            public void afterTextChanged(Editable editable){
                if(isEmpty(blueText) == false){
                    //intFat = Integer.parseInt(etFat.getText().toString());
                    blueBar.setProgress(Integer.parseInt(blueText.getText().toString()));
                }
                else{
                    blueText.setText("0");
                    blueText.hasFocus();
                }
                blueText.setSelection(blueText.getText().length());
                if(Integer.parseInt(blueText.getText().toString()) > 255){
                    blueText.setText("255");
                }
                /*
                try{
                    btOutputStream.write(0b00000011);
                    btOutputStream.write(blueText.getText().toString().getBytes());
                }catch(Exception e){

                }*/
            }
        });
        //send data after user has unfocused from the textEdit
        blueText.setOnFocusChangeListener(new OnFocusChangeListener() {
            public void onFocusChange(View v, boolean hasFocus) {
                if(!hasFocus) {
                    if(myBA != null){
                        if(myBA.isEnabled()){
                            try{
                                btOutputStream.write(0b00000011);
                                //change bytes of chars in redText to a single byte to make
                                //parsing of the text easier on the purse side.
                                byte[] blueOutput = blueText.getText().toString().getBytes();
                                String blueTransmited = "";
                                for(int i=0; i<blueOutput.length;i++){
                                    blueTransmited += Integer.toString((int)blueOutput[i]-48); //"-48" changes ascii into regular numbers.
                                }
                                //since bytes in java are signed and do not go up to 255
                                //the hardware multiplies output1 by 2 and then adds output2
                                //to get the new setting for the red value.
                                byte blueoutput1 = (byte)(Integer.parseInt(blueTransmited)/2);
                                byte blueoutput2 = (byte)(Integer.parseInt(blueTransmited)%2);
                                btOutputStream.write(blueoutput1);
                                btOutputStream.write(blueoutput2);
                                btOutputStream.write(0b00000000);

                            }catch(IOException e){
                                Log.e("error!", ""+e);
                            }
                        }
                    }
                }
            }
        });

        //Brightness Slider//////////////////////////////
        brightBar = findViewById(R.id.brightnessSeekBar);
        brightBar.setOnSeekBarChangeListener(new brightListener());
        brightText = findViewById(R.id.editBrightness);
        brightText.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {}
            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                //brightBar.setProgress(Integer.parseInt(charSequence.toString()));
            }
            @Override
            public void afterTextChanged(Editable editable){
                if(isEmpty(brightText) == false){
                    //intFat = Integer.parseInt(etFat.getText().toString());
                    brightBar.setProgress(Integer.parseInt(brightText.getText().toString()));
                }
                else{
                    brightText.setText("0");
                    brightText.hasFocus();
                }
                brightText.setSelection(brightText.getText().length());
                if(Integer.parseInt(brightText.getText().toString()) > 100){
                    brightText.setText("100");
                }
                /*
                try{
                    btOutputStream.write(0b00000100);
                    btOutputStream.write(brightText.getText().toString().getBytes());
                }catch(Exception e){

                }*/
            }
        });
        //send data after user has unfocused from the textEdit
        brightText.setOnFocusChangeListener(new OnFocusChangeListener() {
            public void onFocusChange(View v, boolean hasFocus) {
                if(!hasFocus) {
                    if(myBA != null){
                        if(myBA.isEnabled()){
                            try{
                                btOutputStream.write(0b00000011);
                                //change bytes of chars in redText to a single byte to make
                                //parsing of the text easier on the purse side.
                                byte[] brightOutput = brightText.getText().toString().getBytes();
                                String brightTransmited = "";
                                for(int i=0; i<brightOutput.length;i++){
                                    brightTransmited += Integer.toString((int)brightOutput[i]-48); //"-48" changes ascii into regular numbers.
                                }
                                //since bytes in java are signed and do not go up to 255
                                //the hardware multiplies output1 by 2 and then adds output2
                                //to get the new setting for the red value.
                                byte brightoutput1 = (byte)(Integer.parseInt(brightTransmited)/2);
                                byte brightoutput2 = (byte)(Integer.parseInt(brightTransmited)%2);
                                btOutputStream.write(brightoutput1);
                                btOutputStream.write(brightoutput2);
                                btOutputStream.write(0b00000000);

                            }catch(IOException e){
                                Log.e("error!", ""+e);
                            }
                        }
                    }
                }
            }
        });

        myPreferences = PreferenceManager.getDefaultSharedPreferences(this);
        editor = myPreferences.edit();
        editor.apply();

        addListenerOnButtonClick();


    }



    //returns true if EditText is empty
    private boolean isEmpty(EditText etText){
        if(etText.getText().toString().trim().length() > 0 )
            return false;
        else
            return true;
    }
    public void addListenerOnButtonClick(){
        btTB = findViewById(R.id.connectButton);

        btTB.setOnCheckedChangeListener(new CompoundButton.OnCheckedChangeListener() {
            public void onCheckedChanged(CompoundButton buttonView, boolean isChecked) {
                if(noBT != 1){
                    if (isChecked) {
                        // The toggle is enabled
                        btON();
                    } else {
                        // The toggle is disabled
                        btOFF();
                    }
                }
            }
        });

        onOff = findViewById(R.id.onOffButton);
        onOff.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View view) {
                try{
                    btOutputStream.write(0b00000101);
                }catch(Exception e){

                }
            }
        });

    }

    private class redListener implements SeekBar.OnSeekBarChangeListener{
        public void onProgressChanged(SeekBar myBar, int progress, boolean fromUser){
            redText.setText(""+progress);
        }
        public void onStartTrackingTouch(SeekBar seekBar) {}
        public void onStopTrackingTouch(SeekBar seekBar) {}
    }
    private class greenListener implements SeekBar.OnSeekBarChangeListener{
        public void onProgressChanged(SeekBar myBar, int progress, boolean fromUser){
            greenText.setText(""+progress);
        }
        public void onStartTrackingTouch(SeekBar seekBar) {}
        public void onStopTrackingTouch(SeekBar seekBar) {}
    }
    private class blueListener implements SeekBar.OnSeekBarChangeListener{
        public void onProgressChanged(SeekBar myBar, int progress, boolean fromUser){
            blueText.setText(""+progress);
        }
        public void onStartTrackingTouch(SeekBar seekBar) {}
        public void onStopTrackingTouch(SeekBar seekBar) {}
    }
    private class brightListener implements SeekBar.OnSeekBarChangeListener{
        public void onProgressChanged(SeekBar myBar, int progress, boolean fromUser){
            brightText.setText(""+progress);

            //change brightness as slider is moved
            if(myBA != null){
                if(myBA.isEnabled()){
                    try{
                        btOutputStream.write(0b00000011);
                        //change bytes of chars in redText to a single byte to make
                        //parsing of the text easier on the purse side.
                        byte[] brightOutput = brightText.getText().toString().getBytes();
                        String brightTransmited = "";
                        for(int i=0; i<brightOutput.length;i++){
                            brightTransmited += Integer.toString((int)brightOutput[i]-48); //"-48" changes ascii into regular numbers.
                        }
                        //since bytes in java are signed and do not go up to 255
                        //the hardware multiplies output1 by 2 and then adds output2
                        //to get the new setting for the red value.
                        byte brightoutput1 = (byte)(Integer.parseInt(brightTransmited)/2);
                        byte brightoutput2 = (byte)(Integer.parseInt(brightTransmited)%2);
                        btOutputStream.write(brightoutput1);
                        btOutputStream.write(brightoutput2);
                        btOutputStream.write(0b00000000);

                    }catch(Exception e){
                        Log.e("error!", ""+e);
                    }
                }
            }
        }
        public void onStartTrackingTouch(SeekBar seekBar) {}
        public void onStopTrackingTouch(SeekBar seekBar) {}
    }

    /*
    on app resume:
    If bt is off on the phone, then ask the user to turn on bluetooth.
    if user says no, then dont initialize bluetooth.
    if user says yes, then initialize bluetooth.
    attempt to connect to bluetooth module.
    if couldnt connect, then output: "could not connect to purse"
    if connection occurs then output "purse connected"
     */
    public void onResume(){
        super.onResume();
        loadProgressRed = myPreferences.getInt(RED_PROGRESS, 0);
        loadProgressGreen = myPreferences.getInt(GREEN_PROGRESS, 0);
        loadProgressBlue = myPreferences.getInt(BLUE_PROGRESS, 0);
        loadProgressBright = myPreferences.getInt(BRIGHT_PROGRESS, 0);
        Log.d("bewp", ""+loadProgressRed);
        redBar.setProgress(loadProgressRed);
        greenBar.setProgress(loadProgressGreen);
        blueBar.setProgress(loadProgressBlue);
        brightBar.setProgress(loadProgressBright);


        //bluetooth initialization code.///////////////////////////////////////
        myBA = BluetoothAdapter.getDefaultAdapter();

        //if bluetooth adapter is on already then set toggle button to on:
        //if already off, set to off.
        //if myBA is null, then there's no bluetooth, like in an emulator.
        if(myBA != null){
            if(myBA.isEnabled()){

                try{
                    btTB = findViewById(R.id.connectButton);
                    btTB.setChecked(true);
                    initBlueTooth(myBA);
                    Log.d("miaumiau4", "");
                }catch(IOException ex){

                }
            }else{
                btTB = findViewById(R.id.connectButton);
                btTB.setChecked(false);
                Log.d("error", "Bluetooth is disabled.");
                Toast.makeText(ctx, "Your bluetooth is disabled!", Toast.LENGTH_LONG).show();
            }
        }else{
            noBT = 1;
            btTB = findViewById(R.id.connectButton);
            btTB.setChecked(false);
            Log.d("rawrrawr!", ""+savedProgressRed);
            Toast.makeText(ctx, "Bluetooth not found.", Toast.LENGTH_LONG).show();
        }
    }
    protected void onPause(){
        super.onPause();
        savedProgressRed = redBar.getProgress();
        savedProgressGreen = greenBar.getProgress();
        savedProgressBlue = blueBar.getProgress();
        savedProgressBright = brightBar.getProgress();
        Log.d("rawr!", ""+savedProgressRed);
        editor.putInt(RED_PROGRESS, savedProgressRed);
        editor.putInt(GREEN_PROGRESS, savedProgressGreen);
        editor.putInt(BLUE_PROGRESS, savedProgressBlue);
        editor.putInt(BRIGHT_PROGRESS, savedProgressBright);
        editor.commit();

        resetConnection();
    }

    private void resetConnection(){
        if (btInStream != null) {
            try {btInStream.close();} catch (Exception e) {}
            btInStream = null;
        }

        if (btOutputStream != null) {
            try {btOutputStream.close();} catch (Exception e) {}
            btOutputStream = null;
        }

        if (btSocket != null) {
            try {btSocket.close();} catch (Exception e) {}
            btSocket = null;
        }
    }


    public void btON(){
        if (!myBA.isEnabled()) {
            Intent turnOn = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
            startActivityForResult(turnOn, 0);
            Log.d("lion1", ""+savedProgressRed);
            //Toast.makeText(getApplicationContext(), "Turned on",Toast.LENGTH_LONG).show();
            /*
            if(!((Activity) getApplicationContext()).isFinishing()){
                //show dialog

                try {
                    //ShowAgilDialog();
                    Toast.makeText(getApplicationContext(), "Bluetooth has been Enabled",Toast.LENGTH_LONG).show();
                    //Toast doesnt work in android 1.7

                } catch (WindowManager.BadTokenException e) {
                    Log.e("WindowManagerBad ", e.toString());
                }

            }*/
        } else {
            Log.d("lion2", ""+savedProgressRed);
            Log.d("lion2", ""+MainActivity.this);
            //Toast.makeText(MainActivity.this, "Bluetooth already Enabled.", Toast.LENGTH_LONG).show();
            findViewById(R.id.MainView).post(new Runnable() {
                public void run() {
                    Toast.makeText(MainActivity.this, "Bluetooth already Enabled.", Toast.LENGTH_LONG).show();
                }
            });
            /*
            if(!((Activity) getApplicationContext()).isFinishing()){
                //show dialog

                try {
                    //ShowAgilDialog();
                    Toast.makeText(getApplicationContext(), "Bluetooth already Enabled.",Toast.LENGTH_LONG).show();
                    //Toast doesnt work in android 1.7

                } catch (WindowManager.BadTokenException e) {
                    Log.e("WindowManagerBad ", e.toString());
                }

            }*/
        }
    }
    public void btOFF(){
        myBA.disable();
        Log.d("lion3", ""+savedProgressRed);
        Toast.makeText(getApplicationContext(), "Turned off" ,Toast.LENGTH_LONG).show();

        /*
        if(!((Activity) ctx).isFinishing()){

            try {
                //ShowAgilDialog();
                Toast.makeText(ctx, "Turned off" ,Toast.LENGTH_LONG).show();
            } catch (WindowManager.BadTokenException e) {
                Log.e("WindowManagerBad ", e.toString());
            }
        }*/
    }

    @Override
    public boolean dispatchTouchEvent(MotionEvent event) {
        if (event.getAction() == MotionEvent.ACTION_DOWN) {
            View v = getCurrentFocus();
            if ( v instanceof EditText) {
                Rect outRect = new Rect();
                v.getGlobalVisibleRect(outRect);
                if (!outRect.contains((int)event.getRawX(), (int)event.getRawY())) {
                    v.clearFocus();
                    InputMethodManager imm = (InputMethodManager) getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
                }
            }
        }
        return super.dispatchTouchEvent( event );
    }

    private void initBlueTooth(BluetoothAdapter myBA) throws IOException {
        //BluetoothAdapter blueAdapter = BluetoothAdapter.getDefaultAdapter();
        if (myBA != null) {
            if (myBA.isEnabled()) {
                Set<BluetoothDevice> bondedDevices = myBA.getBondedDevices();
                if(bondedDevices.size() > 0) {
                    Object[] devices = (Object []) bondedDevices.toArray();
                    ArrayList deviceNamesList = new ArrayList();
                    for(BluetoothDevice bt : bondedDevices) deviceNamesList.add(bt.getName());
                    //if the purse is not connected, send alert saying that its not connected or something.
                    if(deviceNamesList.indexOf("AITSmartPurse") < 0){
                        Toast.makeText(ctx, "AITSmartPurse is not a bonded device!",Toast.LENGTH_LONG).show();
                    }else {

                        try{
                            Log.d("miau3", ""+savedProgressRed);
                            BluetoothDevice device = (BluetoothDevice) devices[deviceNamesList.indexOf("AITSmartPurse")];
                            ParcelUuid[] uuids = device.getUuids();
                            btSocket = device.createRfcommSocketToServiceRecord(uuids[0].getUuid());
                            btSocket.connect();
                            btOutputStream = btSocket.getOutputStream();
                            btInStream = btSocket.getInputStream();
                            Toast.makeText(ctx, "Connected to AITSmartPurse! c:",Toast.LENGTH_LONG).show();
                            Log.d("miau4", ""+savedProgressRed);
                        }catch (IOException e1){
                            if(btSocket != null){
                                try{
                                    btSocket.close();
                                }catch(IOException e2){
                                    e2.printStackTrace();
                                }
                                btSocket = null;
                            }

                            e1.printStackTrace();
                            if(!((Activity) ctx).isFinishing()){
                                Toast.makeText(ctx, "Could not connect to AITSmartPurse!",Toast.LENGTH_LONG).show();
                            }

                        }


                        //turn on a light or something to show it's connected.
                    }
                }

                Log.e("error", "No appropriate paired devices.");
            } else {
                Log.e("error", "Bluetooth is disabled.");
                Toast.makeText(ctx, "Your bluetooth is disabled", Toast.LENGTH_LONG).show();
            }
        }
    }


}

