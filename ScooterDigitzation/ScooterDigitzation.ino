#include <EEPROM.h>

#define ADSELVOL 0x20  // ADC channel 0 for bettery voltage
#define ADSELFUEL 0x21 // ADC channel 1 for fuel
#define ADSETEMP 0x22  // ADC channel 2 for ambient temperature
#define SYSTMMAX   30000;

struct ScooterVar
{
  unsigned int tach;
  unsigned char speed;
  unsigned char fuel;
  unsigned char battery;
  unsigned char temp;
  int etemp;
 /* data */
};


bool tempSmp = 0;
bool eTempSmp = 0;
bool fuelSmp = 0;
bool batterySmp = 0;
bool tachSmp = 0;
bool speedSmp = 0;

unsigned char fuelSmpTM = 0;			// fuel sample counter 
unsigned char tempSmpTM = 0;			// temperature sample counter 
unsigned char batterySmpTM = 0;			// voltage sample counter 
unsigned char speedSmpTM = 0;

unsigned int nSysT = 0;
unsigned int nTemp = 0;
unsigned int nETemp = 0;
unsigned int nSpeed = 0;
unsigned int nFuel = 0;
unsigned int nBattery = 0;
unsigned int nTach = 0;

void rpm (){
  nTach++；
}

void setup() {
uint8_t tmp;
unsigned int nTimeTmp;
ScooterVar scorVar;
  T2_init()；
  attachInterrupt(0, rpm, RISING);
  sei();
  ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADEN);  //ADC Enable ADC 1/64= 125K
  ADMUX = ADSELVOL;  // ADC set channel 0
  batterySmp = 1; //enable voltage sample
  ADCSRA |= (1<<ADSC);  //Star ADC 

}

void loop() {

        if (((ADCSRA & (1<<ADSC)) != 0) && (batterySmp != 0))
        {
          tmp = ADCH;
          nBattery += tmp;
          batterySmpTM++;
          ADMUX = ADSELFUEL;
          batterySmp = 0;
          speedSmp = 0;
          tempSmp = 0;
          fuelSmp = 0;
          if(batterySmpTM <= 8){
            scorVar.battery = (unsigned char)(nBattery >> 3);
            batterySmpTM = 0;
            nBattery = 0;
          }        
          _delayus(30);
          ADCSRA |= (1<<ADSC);
        }
 




    if(tachSmp && firstT)
    {
      		cli();
					nOnTM = nSysT;
      firstTach =  nTach;

					sei();
          secondT = 1;
          firstT = 0;
    }
    if(tachSmp && secondT && (nOnTM % 500 == 0))
    {
      cli();
      nTach = 10000 + nTach - firstTach;
      sei();
      		if(nTach >= 10000)
					{
						nTach = nTach - 10000;
					}	
      scorVar.tach = nTach;
    }
}

void T2_init(){
  OCR = 0x00;
  TCNT2 =0x00;
  TCCR2 = 0x03;  
  TIMSK |= (1 << TOIE2); 
}

ISR(TIMER2_OVF_vect)
{
      nSysT++;
  		if(nSysT >= SYSTMMAX)
		{
			nSysT = 0;
		}

    if (nTach >= 10000)
    {
      nTach = 0;
    }
}