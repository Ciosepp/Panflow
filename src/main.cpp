#include <Arduino.h>

#include <max6675.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Timer.h>
#include <rEncoder.h>

#define rateCalcTime 1000


#define HEAT_PIN 9
#define COOL_PIN 8

#define ENCA_PIN 3
#define ENCB_PIN 4
#define ENCP_PIN 2

rEncoder encoder(ENCA_PIN, ENCB_PIN, ENCP_PIN, false);

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int thermoDO = 12;
int thermoCS = 11;
int thermoCLK = 10;

MAX6675 T0(thermoCLK, thermoCS, thermoDO);
int readTemp;
int TEMPERATURE=0;
int oldTemperature =0;
int TEMP_Difference =0;

Timer rateCalcTimer(MILLIS);
Timer Temp_sampling(MICROS);
Timer T1;
bool t1000;

Timer heatPwmTimer;


int a=0;

int lastSampleTime;
void getTemp(bool avg = false, int n = 10){
    if (Temp_sampling.timerClock(250000))
    {
        if(avg)
        {
			if (a==0)
			{
				TEMPERATURE = readTemp/n;
				readTemp =0;
                TEMP_Difference = oldTemperature - TEMPERATURE;
                oldTemperature = TEMPERATURE;
			}
			
			readTemp += T0.readCelsius()*10;
			lastSampleTime = millis();
			a=((a+1)%n);
    	}    
        else{
            TEMPERATURE = T0.readCelsius()*10;
            
        }
    }
    
}  
//v= sqr(x)
//p = v2/r
//p = (sqrt(x))2 = x
#define VP_scaling 1
int VP_linearization(int powerPerc){
    return (int)sqrtf(powerPerc)*VP_scaling;
}

int POWER = 0;
int error = 0;
int lastTemp =0;
long integralTimeBuffer =0;

// int PIDControl(int setPoint, int P, int I, int D){
//     error = setPoint - TEMPERATURE;
//     if (millis() - integralTimeBuffer >= 1000) {
//         acc += error;
//         derivate = TEMPERATURE - lastTemp;
//         lastTemp = TEMPERATURE;
//         integralTimeBuffer = millis();
//     }    

//     POWER = error * P + acc * I + derivate * D;
//     return POWER;
// }

void timersInit(){
    rateCalcTimer.setDelayTime(rateCalcTime);
    rateCalcTimer.startDelay();
}
int oldTemp = 0;
int rate= 0;
int rateControl(int targetRate){
    int cTemp = T0.readCelsius()*10;

    if (rateCalcTimer.getDelayQ())
    {
        rate = cTemp - oldTemp;
        oldTemp = cTemp;
    }
    return rate;
}

int memTemp=0;
void printTemp(){
    if (T1.timerClock(1000000))
    {
        lcd.setCursor(12,0);
        lcd.print("     ");
        lcd.setCursor(12,0);
	    lcd.print(TEMPERATURE);
    }    
    
}    

int heatRate = 10;//[]
int reflowTemp = 220;
int peakTemp = 250;
int reflowTime = 60;
uint32_t heatPWMPeriod = 2000000;

void heaterController(bool ENABLE,int power){// power: 0 - 1000
	uint32_t ton = power;
	(ton > 1000) ? ton = 1000 :ton;
    (ton < 1 ) ? ton = 0 : ton;
	if (ENABLE)
	{
		if (heatPwmTimer.timerClock(heatPWMPeriod))
		{
			heatPwmTimer.TriggMonoTimer(ton);
             Serial.println(heatPWMPeriod);
		}
		digitalWrite(HEAT_PIN, heatPwmTimer.getMonoQ());

        Serial.println(ton);
	}
	else{
		digitalWrite(HEAT_PIN,0);
	}
	
}



void setup() {

    Wire.begin();
    Wire.beginTransmission(0x27);
    lcd.begin(16, 2);
    
    lcd.clear();
    lastTemp = readTemp;
    lcd.setCursor(0,0);

    lcd.setBacklight(255);
    lcd.print("Temperature:      C");

    pinMode(COOL_PIN, OUTPUT);
    pinMode(HEAT_PIN, OUTPUT);
    delay(500);
    Serial.begin(9600);
}
int y=0;

void loop() {
    
    getTemp(true);
    printTemp();
    
    digitalWrite(COOL_PIN,!digitalRead(ENCP_PIN));
	 y = encoder.updateEncoder(y);
    //Serial.println(y);
    heaterController(true, y);
}

