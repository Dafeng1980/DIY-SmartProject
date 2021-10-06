/*
 * Ardunio NANO Mega128模块，参考选用：https://github.com/MCUdude/MegaCore
 * 使用了 SG90伺服马达 和 HC-SR04超声波感应器 两个器件
 * Mega128 模块设置External 8Mhz。 
 */
#include <Servo.h>   //servo library 
Servo servo;
int kPowerPin = 4;     //定义模块关机控制引脚
int kTrigPin = 5;      //定义超声波控制引脚
int kEchoPin = 6;   
int kServoPin = 7;     //定义伺服马达控制引脚
int kLed= 13;          //定义红色LED控制引脚
int kBuzzerPin = 15;   //定义嗡鸣器控制引脚
long duration, dist, average;   
long aver[8];   //array for average
uint32_t  shutdownmillis = 0;


void setup() {
    pinMode(kTrigPin, OUTPUT);      
    pinMode(kEchoPin, INPUT); 
    pinMode(kBuzzerPin, OUTPUT);
    pinMode(kLed, OUTPUT);
    pinMode(kPowerPin, OUTPUT);
    digitalWrite(kPowerPin,HIGH);   //关机控制引脚拉高，给模块供电
    delay(30);          
    Serial.begin(38400);
    servo.attach(kServoPin);  //设置伺服马达的初始位置。桶盖关闭。
    servo.write(0);          //close cap on power on
    delay(200);
    servo.detach(); 
    digitalWrite(kLed,HIGH);  //开启红色LED指示灯
    shutdownmillis = millis(); //启动定时关机，毫秒计数器 
    delay(100);
} 


void loop() { 
  for (int i=0;i<=8;i++) {   //average distance
    measure();               // 循环8次测量超声波数据。
    aver[i]=dist;            
    delay(10);              //delay between measurements
  }
  dist=(aver[0]+aver[1]+aver[2]+aver[3]+aver[4]+aver[5]+aver[6]+aver[7]) >> 3; // 计算8次超声波测距的距离和。并计算平均值。     

if ( dist < 20 ) {               // 如果超声波距离小于20厘米，就执行桶开盖。
//Change distance as per your need
 digitalWrite(kLed,LOW);        // 开桶盖，关闭LED指示灯
 servo.attach(kServoPin);      // 调整伺服马达的角度，以开启桶盖。
 delay(1);
 servo.write(30);
 delay(100);
 buzzer();                    // 嗡鸣响声一次
 servo.write(40);
 delay(100);
 servo.write(50);
 delay(100);
 servo.write(60);
 delay(100);
 servo.write(70);
 delay(100);
 servo.write(80);
 delay(100);
 servo.write(90);     //  伺服马达 调整到90度  桶盖开启
  delay(100);
 servo.write(110);
 delay(3000);         //  开启桶盖，延时3秒  
 
 servo.write(60);     //  延时3秒后，开始关闭桶盖  
 delay(100);
 buzzer();            // 嗡鸣响声一次
 servo.write(40);
 delay(150);
 servo.write(30);
 delay(150);
 servo.write(20);
 delay(150);
 servo.write(10);
 delay(150);
 servo.write(0);     //  伺服马达 调整到0度  桶盖关闭
 delay(1000);        //  关闭桶盖，延时1秒 
 servo.detach();
 digitalWrite(kLed,HIGH);  //桶盖关闭后，开启LED指示灯
 shutdownmillis = millis();   //重新获取新的定时关机毫秒数据   
}

if((millis() - shutdownmillis) / 1000 >= 60){   // 如果定时计数器超过60秒，就执行关机。以达到节省锂电池的电
//   Serial.print("dist =");
//   Serial.println(dist + 200);
   digitalWrite(kPowerPin,LOW);          //关机控制引脚拉低，整个模块无供电 需要重新按开关按键
   delay(500);
}

}

void measure() {     //超声波测距函数方程
digitalWrite(kTrigPin, LOW);
delayMicroseconds(5);
digitalWrite(kTrigPin, HIGH);
delayMicroseconds(15);
digitalWrite(kTrigPin, LOW);
pinMode(kEchoPin, INPUT);
duration = pulseIn(kEchoPin, HIGH);
dist = (duration/2) / 29.1;    //obtain distance
}

void buzzer(){      //嗡鸣响声函数方程
         tone(kBuzzerPin, 3980);
          delay(80);
           noTone(kBuzzerPin);
    }
