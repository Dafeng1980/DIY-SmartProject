
/*
基于AVR mega128L 的智能家居系统
光敏， 温度，等感应器
运行7.3728Mhz外置晶振，BOD关闭。bootloader：UART0，115200下载速率。
 Ardunio硬件设置基于：https://github.com/MCUdude/MegaCore

  Created:     2020/2/8 
  Author:     Dafeng / Zhangcan  
 */
 
#include <Servo.h>
#include <avr/wdt.h>
#include <EEPROM.h>

Servo myservo;  

volatile uint16_t cycle;                   //循环次数，记录于内置EEPROM, 地址0x00 断电记忆
int pos;                                  //伺服电机的位置，角度。 记录于内置EEPROM，地址0x10； 断电记忆
bool posstatus;                            
uint8_t homestatus;

const int kLightSensorLPin = A7;           // 高亮传感， 光敏电阻引脚, 数值越大，光亮度越大，（0-1023）
const int kLightSensorHPin = A6;           // 低暗传感， 光敏电阻引脚，数值越小， 光亮度越小， （0-1023）
const int kTemperaturePin = A5;            //温度传感，  硅温度正温度引脚   KTY82-110 硅温度传感器
const int kBatteryPin = A3;                //电池电压，  监控电池电量引脚

const int kLightPin = 13;                 //灯控制, 光度低时自动点亮，越暗越亮，光度正常时自动关闭。
const int kServoPin = 15;		              //伺服马达驱动引脚，控制太阳能板，获取光能转换最佳效率。 伺服马达 SG90
const int kAirConditionPushPin = 23;      //空调控制推引脚， 温度大约低于17摄氏度，马达顺时针转动，制暖。 使用手机的震动马达
const int kAirConditionPullPin = 24;      //空调控制拉引脚， 温度大约高于29摄氏度，马达逆时针转动，制冷。
const int kPowerSavePin = 25;             //关闭外置传感器供电，更低的待机功耗!

uint16_t read_light_l, read_light_h, read_temp, read_battery;
int read_diff;

void setup() {
  pinMode(kLightPin, OUTPUT);
    pinMode(kPowerSavePin, OUTPUT);  
      pinMode(kAirConditionPushPin, OUTPUT);
        pinMode(kAirConditionPullPin, OUTPUT);
    portinit();
  
      pinMode(kLightSensorLPin, INPUT);
        pinMode(kLightSensorHPin, INPUT);
          pinMode(kTemperaturePin, INPUT);
            pinMode(kBatteryPin, INPUT);
              analogReference(INTERNAL);    //基准2.63V

      myservo.attach(kServoPin);
        EEPROM.get(0x10, pos);
          if(pos >90 && pos < 1)
          {
            pos = 3;
            EEPROM.put(0x10, pos);
          }
            myservo.write(pos);
  
    EEPROM.get(0x00,cycle);
        if ( cycle > 900 ){
          cycle = 0;
          EEPROM.put(0x00, cycle);
        }
  Serial.begin(115200);
    Serial.println("  Reset_Print_Test: ");
    delay(100);
      homestatus = 0;
        posstatus = true;
}

void loop() {    
    digitalWrite(kPowerSavePin, HIGH);
      getallval();
        cycle++;
          Serial.print("POS_STATUS: ");
          Serial.println(posstatus);
          delay(200);
            if (posstatus)  detectsensors(); 
              Serial.print("=============================CYCLE: ");
              Serial.println(cycle);
                delay(300);
              Serial.print("===HOMESTATUS: ");
              Serial.println(homestatus);
                delay(300);
        detectpower();
    switch (homestatus){
      case 0:                     //状态0， 正常状态，自动检测各种信号，和每隔一段时间调整太阳能板位置。运行10分钟后关机。
          if (cycle >= 101){         
              cycle = 0;
              EEPROM.put(0x00, cycle);
              powerdown();
          }

    	  if (cycle == 1 || !(cycle % 100)) {
    		  posstatus = false;
    		  homestatus = 1;
    	  }	  
        else {
    		  EEPROM.put(0x00, cycle);
    		  powerstandby();
          }
    			break;
      
      case 1:                  //状态1，自动调整太阳能板位置一次
  		  setpos();
  		  posstatus = true;
  		  homestatus = 0;
  			break;
      
	  case 2:                   //状态2，晚上（光感应值）或不在舒适温度范围（温度感应值）；运行工作30分种后关机。
		  if (cycle >= 450) {
			  cycle = 0;
			  EEPROM.put(0x00, cycle);
			  powerdown();
		  }
			if (!(cycle % 300))
     {
          posstatus = false;
          homestatus = 1;
     }
			break;
      
    case 3:                                     //状态3， 电池电量低，灯闪烁6秒 (提示要充电）后，马上关机。
        for (int i = 0; i<9 ; i++){
          digitalWrite(kLightPin, HIGH);
          delay(180);
          digitalWrite(kLightPin, LOW);
          delay(520);      
        }
      		powerdown();
      		break;
      
    default:
		  	powerdown();
			break;
    }
	delay(1000);
   }

 void getallval()    //获取所有传感器的数值:
  {      
        read_light_l = analogRead(kLightSensorLPin);
          Serial.print("Light_L_Value: ");
          Serial.println(read_light_l);
            delay(10);
        read_light_h =analogRead(kLightSensorHPin);
          Serial.print("Light_H_Value: ");
          Serial.println(read_light_h);
            delay(10);
        read_temp = analogRead(kTemperaturePin);
          Serial.print("Temperead_batteryrature: ");
          Serial.println(read_temp);
            delay(10);
        read_battery = analogRead(kBatteryPin); 
          Serial.print("Bttery: ");
          Serial.println(read_battery);
            delay(10); 
    }
 void getsensors() {
	 read_light_l = analogRead(kLightSensorLPin);
	  read_light_h = analogRead(kLightSensorHPin);
	    read_temp = analogRead(kTemperaturePin);
	      read_diff = read_light_h - read_light_l;
       }

void portinit(){
  digitalWrite(kLightPin, LOW);
    digitalWrite(kPowerSavePin, LOW);
      digitalWrite(kAirConditionPullPin, LOW);   //  高电平，电机顺时针
        digitalWrite(kAirConditionPushPin, LOW);   //  高电平，电机逆时针
    }
 void detectsensors(){
		bool tempstatus = true;
		    getsensors();
          if(read_light_h < 300 && read_diff < 200){
                uint8_t lightvalue = read_light_h;
                  lightvalue = map(lightvalue, 20, 380, 255, 0);
                    analogWrite(kLightPin, lightvalue);
                      homestatus = 2;
                        tempstatus = false; 
                  }
                else if (read_light_h > 350 && read_diff > 220){
                          digitalWrite(kLightPin, LOW);
                            tempstatus = true;
                              homestatus = 0;
                  }
       
            if ( read_temp < 830 ) {
                  digitalWrite(kAirConditionPushPin, HIGH);
                  digitalWrite(kAirConditionPullPin, LOW);
                    homestatus = 2;
            }
            if (read_temp > 858) {
                  digitalWrite(kAirConditionPullPin, HIGH);
                  digitalWrite(kAirConditionPushPin, LOW);
                    homestatus = 2;		
            }
            if(read_temp >= 833 && read_temp <= 855)
            {
                digitalWrite(kAirConditionPushPin, LOW);
                digitalWrite(kAirConditionPullPin, LOW);
                  if(tempstatus)  homestatus = 0;
            }
      }

void detectpower(){
		read_battery = analogRead(kBatteryPin);
       if (read_battery < 450 )  homestatus = 3;   	 
}

void setpos(){
		uint16_t firstl, firsth ;
		int diffl, diffh;
      for (pos = 3; pos <= 90; pos += 1) { 
              myservo.write(pos);             
              delay(15);                       
        }
      for (pos = 90; pos >= 3; pos -= 1) { 
          myservo.write(pos);              
          delay(15);                       
      }
        EEPROM.get(0x10, pos);
          Serial.print("GET-UP--Position--- = ");
            Serial.println(pos);
              delay(500);
                  for (pos; pos <= 90; pos += 1)
                  { 
                        myservo.write(pos);
                          Serial.print("-UP--Position--- = ");
                          Serial.println(pos);
                          //delay(300);
                        delay(1200);
                        getsensors();
                          firstl = read_light_l;
                          firsth = read_light_h;
                          pos = pos + 2;
                          myservo.write(pos);
                          Serial.print("-UP-NEXT-Position--- = ");
                          Serial.println(pos);
                          //  delay(300);
                        delay(1200);
                        getsensors();
                        diffl = read_light_l - firstl;
                        diffh = read_light_h - firsth;
                            Serial.print("DIFFL = ");
                            Serial.println(diffl);
                            delay(200);
                            Serial.print("DIFFH = ");
                            Serial.println(diffh);
                            delay(200);
                        if(diffh <= 0 && diffl <= 0)
                          break;
                  }
        for (pos; pos >= 3; pos -=1) {
            myservo.write(pos);
              delay(1200);
                getsensors();
                  firstl = read_light_l;
                  firsth = read_light_h;     
                    pos = pos - 2;
                    myservo.write(pos);
                      delay(1200);
                      getsensors();
                        diffl = read_light_l - firstl;
                        diffh = read_light_h - firsth;
                          if(diffh <= 0 && diffl <= 0)  break;
                  }
        
        Serial.print("-----------------------Position--- = ");
              Serial.println(pos);
              delay(300);
              EEPROM.put(0x10, pos);
    }

  void powerdown()   // 控制器省电关机模式.
  {
  		 // digitalWrite(kPowerSavePin, LOW);
          portinit();
    		  ADCSRA &= ~(1 << ADEN);
    		  MCUCSR |= (1 << JTD);
  
		// if (period != SLEEP_FOREVER)
		//   {
		// 		WDTCR |= (1 << WDCE) | (1 << WDE);
		// 		WDTCR = (1 << WDP2) | (1 << WDP1) | (1 << WDP0);   // 约2秒看门狗RESET设置
		// 		WDTCR |= (1 << WDE);
		//   }
     
		      set_sleep_mode(SLEEP_MODE_PWR_DOWN);
		        cli();
		          sleep_enable();
  		          sei();
  		            sleep_cpu();   
      		            sleep_disable();
                        sei();
                        ADCSRA |= (1 << ADEN);
  }

void powerstandby(){       // 控制器省电待机模式.
        portinit();
          ADCSRA &= ~(1 << ADEN);
          MCUCSR |= (1 << JTD);  
            WDTCR |= (1 << WDCE) | (1 << WDE);
            WDTCR = (1 << WDP2) | (1 << WDP1) | (1 << WDP0);   // 约2秒看门狗RESET设置
            WDTCR |= (1 << WDE);  
              set_sleep_mode(SLEEP_MODE_STANDBY);
                 cli();
                  sleep_enable();
                  sei();
                    Serial.println("Enter_Cpu_STANDBY_Status: ");
                    delay(100);
                    sleep_cpu();       
                      sleep_disable();
                        WDTCR = (1 << WDCE) | (1 << WDE);
                        WDTCR = 0x00;
                          sei();
                          ADCSRA |= (1 << ADEN);
                }