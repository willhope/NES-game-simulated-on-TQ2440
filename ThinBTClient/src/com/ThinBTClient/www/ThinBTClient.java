package com.ThinBTClient.www;

import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.HandlerThread;
import android.os.Looper;
import android.os.Message;

import java.io.IOException;
import java.io.InputStream;

import java.io.OutputStream;
 
import java.util.UUID;
 

import android.app.Activity;
 
import android.bluetooth.BluetoothAdapter;
 
import android.bluetooth.BluetoothDevice;
 
import android.bluetooth.BluetoothSocket;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
 
import android.os.Bundle;
import android.provider.ContactsContract.CommonDataKinds.Event;
 
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
 
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;
import android.widget.Toast;
public class ThinBTClient extends Activity {
	 
    private static final boolean D = true;
    private BluetoothAdapter mBluetoothAdapter = null;
    private BluetoothSocket btSocket = null;
    private OutputStream outStream = null;
    private InputStream  InStream = null;
    public  boolean BluetoothFlag  = true;
    public int runState = 0;//0:停止  1:前进  2:后退
    public  int tansState = 0;
 //   SensorManager mySensorManager;		//SensorManager对象引用
    
    Button mButtonU; 
    Button mButtonD;
    Button mButtonL;
    Button mButtonR;
    Button mButtonS;
    Button mButtonA;
    Button mButtonB;
    Button mButtonQ;
    Button mButtonO;
    TextView myText;
    SeekBar seekBarPWM;
 

    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");


   // private static String address = "00:13:12:24:18:64"; // <==要连接的蓝牙设备MAC地址
    private static String address = "00:13:12:25:30:94"; // <==要连接的蓝牙设备MAC地址

    
    
    /** Called when the activity is first created. */

    @Override

    public void onCreate(Bundle savedInstanceState) {

            super.onCreate(savedInstanceState);                
            setContentView(R.layout.main);
            
            myText = (TextView) findViewById(R.id.myText);
            myText.setText("蓝牙还不可用,请稍后...");
//            seekBarPWM = (SeekBar) findViewById(R.id.seekBarPWM);
//            seekBarPWM.setMax(100);   
//            seekBarPWM.setProgress(30);
            
//         // 设置拖动条改变监听器   
//            OnSeekBarChangeListener osbcl = new OnSeekBarChangeListener() {        
//                @Override  
//                public void onProgressChanged(SeekBar seekBar, int progress,   
//                        boolean fromUser) {  
//                	int data = seekBarPWM.getProgress();
//                	myText.setText("当前进度：" + data);   
//                	
//                	String message;
//					byte[] msgBuffer = new byte[5];				
//					try {
//                      	outStream = btSocket.getOutputStream();
//
//                      } catch (IOException e) {
//                          e.printStackTrace();
//                      }                     
//                      msgBuffer[0] = 0;
//                      msgBuffer[1] = (byte)(data >> 24 & 0xff);
//                      msgBuffer[2] = (byte)(data >> 16 & 0xff);;
//                      msgBuffer[3] = (byte)(data >> 8 & 0xff);;
//                      msgBuffer[4] = (byte)(data >> 0 & 0xff);;
//                      
//                      try {
//                      	outStream.write(msgBuffer);
//
//                      } catch (IOException e) {
//                    	  e.printStackTrace();
//                      }
//					
//              }   
//      
//                @Override  
//                public void onStartTrackingTouch(SeekBar seekBar) {   
//                   // Toast.makeText(getApplicationContext(), "onStartTrackingTouch",   
//                         //   Toast.LENGTH_SHORT).show();   
//                }   
//      
//                @Override  
//                public void onStopTrackingTouch(SeekBar seekBar) {   
//                   // Toast.makeText(getApplicationContext(), "onStopTrackingTouch",   
//                         //   Toast.LENGTH_SHORT).show();   
//                }   
//      
//            };      
//            // 为拖动条绑定监听器   
//            seekBarPWM.setOnSeekBarChangeListener(osbcl);   

        
            //Up
            mButtonU=(Button)findViewById(R.id.btnU);
            
            mButtonU.setOnTouchListener(new Button.OnTouchListener(){
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						 runState = 1;
//						// sendCmd("a");
//						 
//						 sendCmd("0x10");
//						 Log.d("Season","Up");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//						 runState = 0;
//						// sendCmd("g");
//						 Log.d("Season","停止yunzhuan");
//						break;
//					}
//					
					 sendCmd("0x12");
					 return false;
				} 
					
            });
               
          //Down
            mButtonD=(Button)findViewById(R.id.btnD);
            mButtonD.setOnTouchListener(new Button.OnTouchListener(){
            @Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						runState = 0;//2
//						
//						Log.d("Season","Down");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//						runState = 0;
//					//	sendCmd("g");
//						Log.d("Season","停止yuzhaun");
//						break;
//					}
					sendCmd("0x13");				
					return false;
				}       
            });
          //左
            mButtonL=(Button)findViewById(R.id.btnL);
            mButtonL.setOnTouchListener(new Button.OnTouchListener(){          
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						sendCmd("0x40");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//				//		sendCmd("C");
//						break;
//					}		
					sendCmd("0x14");
					return false;				
	             }
            });
          //右转
            mButtonR=(Button)findViewById(R.id.btnR);
            mButtonR.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						sendCmd("0x80");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//				//		sendCmd("D");
//						break;
//					}		
					sendCmd("0x15");
					return false;
				}
            	
         
            });
            

            //A
            mButtonA=(Button)findViewById(R.id.btnA);
            mButtonA.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						Log.d("Season","A");
//						sendCmd("0x01");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//			//			sendCmd("G");
//						//Log.d("Season","停止");
//						break;
//					}
					sendCmd("0x16");
					return false;
				
				}
         
            });
            
            //B
            mButtonB=(Button)findViewById(R.id.btnB);
            mButtonB.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						Log.d("Season","B");
//						sendCmd("0x02");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//			//			sendCmd("G");
//						//Log.d("Season","停止");
//						break;
//					}
					sendCmd("0x17");
					return false;
				
				}
         
            });
            
            //Start
            mButtonS=(Button)findViewById(R.id.btnS);
            mButtonS.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						Log.d("Season","Start");
//						sendCmd("0x08");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//			//			sendCmd("G");
//						//Log.d("Season","停止");
//						break;
//					}
					sendCmd("0x18");
					return false;
				
				}
         
            });
            
            //quit
            mButtonQ=(Button)findViewById(R.id.btnQ);
            mButtonQ.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
//					int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						Log.d("Season","quit");
//						sendCmd("0x30");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//			//			sendCmd("G");
//						//Log.d("Season","停止");
//						break;
//					}
					sendCmd("0x19");
					return false;
				
				}
         
            });
            
            mButtonO=(Button)findViewById(R.id.btnO);
            mButtonO.setOnTouchListener(new Button.OnTouchListener(){
            
				@Override
				public boolean onTouch(View v, MotionEvent event) {
					// TODO Auto-generated method stub
				int action = event.getAction();
//					switch(action)
//					{
//					case MotionEvent.ACTION_DOWN:
//						Log.d("Season","OK");
//						sendCmd("0x04");
//					break;
//					
//					case MotionEvent.ACTION_UP:
//			//			sendCmd("G");
//						//Log.d("Season","停止");
//						break;
//					}
					//if(action==MotionEvent.ACTION_DOWN)
					sendCmd("0x84");
					return false;
				
				}
         
            });
            
            
            mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
            if (mBluetoothAdapter == null) { 
            	Toast.makeText(this, "蓝牙设备不可用，请打开蓝牙！", Toast.LENGTH_LONG).show();
                    finish();
                    return;
            }

            if (!mBluetoothAdapter.isEnabled()) {
                    Toast.makeText(this,  "请打开蓝牙并重新运行程序！", Toast.LENGTH_LONG).show();
                    finish();
                    return;
            }    
    //        mySensorManager = (SensorManager)getSystemService(SENSOR_SERVICE);	//获得SensorManager对象 
    }

    

    
    @Override
    public void onStart() {
            super.onStart();
    }


    @Override
    public void onResume() {
//          mySensorManager.registerListener(//调用方法为SensorManager注册监听器
//    				mySensorEventListener,			//实现了SensorEventListener接口的监听器对象
//    				mySensorManager.getDefaultSensor(Sensor.TYPE_ORIENTATION),	//Sensor对象
//    				SensorManager.SENSOR_DELAY_UI		//系统传递SensorEvent事件的频度
//    				); 
    	super.onResume();
            
            DisplayToast("正在尝试连接蓝牙设备，请稍后····");
            BluetoothDevice device = mBluetoothAdapter.getRemoteDevice(address);
            try {
               btSocket = device.createRfcommSocketToServiceRecord(MY_UUID);
            } catch (IOException e) {
                 DisplayToast("套接字创建失败！");
            }
            DisplayToast("成功连接蓝牙设备！");
            mBluetoothAdapter.cancelDiscovery();
            try {
                    btSocket.connect();
                 //   DisplayToast("连接成功建立，可以开始操控了~~~");
                    myText.setText("蓝牙设备已准备好了!"); 
                    BluetoothFlag = true;
                    //MyThread bluetoothThread = new MyThread();
                   // bluetoothThread.start();
            } catch (IOException e) {
                   try {
                    	btSocket.close();
                    } catch (IOException e2) {                        
                            DisplayToast("连接没有建立，无法关闭套接字！");
                    }
            }
    }


    @Override
    public void onPause() {
//    	mySensorManager.unregisterListener(mySensorEventListener);	//取消注册监听器
            super.onPause();
            if (outStream != null) {
                    try {
                            outStream.flush();
                    } catch (IOException e) {
                    	 e.printStackTrace();
                    }
            }
            try {
                    btSocket.close();
                    BluetoothFlag = false;//关闭蓝牙读线程
            } catch (IOException e2) {
                    
                    DisplayToast("套接字关闭失败！");
            }
    }


    @Override
    public void onStop() {
            super.onStop();
    }


    @Override
    public void onDestroy() {
            super.onDestroy();

    }
    public void DisplayToast(String str)
    {
    	Toast toast=Toast.makeText(this, str, Toast.LENGTH_LONG);
    	toast.setGravity(Gravity.TOP, 0, 220);
    	toast.show();
    	
    }
   
    public void sendCmd(String message){
    	try {
          	outStream = btSocket.getOutputStream();

          } catch (IOException e) {
             e.printStackTrace();
          }
          byte[] msgBuffer; 
          msgBuffer = message.getBytes();
          try {
          	outStream.write(msgBuffer);
          	
          } catch (IOException e) {
                 e.printStackTrace();
          }	
    }
    
    
    public class  MyThread extends Thread {

		MyThread()
		{	
		  BluetoothFlag = true;
		  try {
                 InStream = btSocket.getInputStream();
                
		          } catch (IOException e) {
		        	  e.printStackTrace();
              }
		}
    	
    	@Override
		public void run() {
			// TODO Auto-generated method stub
    		try{
    			Thread.sleep(100);	
    		}catch (Exception e) {
                e.printStackTrace();
            }
    		
			while(BluetoothFlag){
                     byte[] tmp = new byte[1024];
                     try {             	 
                   	   int len = InStream.read(tmp, 0 , 1024);
                   	   Log.d("Season","len:" + len+new String(tmp,0,1024)  );                   
                        } catch (IOException e) {
                            e.printStackTrace();
                    }
			}	
		}
	
     }
    
}