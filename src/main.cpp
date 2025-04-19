#include <Arduino.h>

#include <max6675.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>
#include <Wire.h>
#include <Timer.h>

#define rateCalcTime 1000


#define HEAT_PIN 9
#define COOL_PIN 8

#define ENCA_PIN
#define ENCB_PIN
#define ENCP_PIN

LiquidCrystal_PCF8574 lcd(0x27);  // set the LCD address to 0x27 for a 16 chars and 2 line display

int thermoDO = 12;
int thermoCS = 11;
int thermoCLK = 10;

MAX6675 T0(thermoCLK, thermoCS, thermoDO);
int readTemp;
int KP = 1;
int kI = 1000;
int KD = 0;

long lastSampleTime=0;
long acc = 0;
long derivate =0;

Timer rateCalcTimer(MILLIS);

void getTemp(){
    if(millis() - lastSampleTime > 200){

        readTemp = T0.readCelsius()*10;
        lastSampleTime = millis();
    }    
}    

int POWER = 0;
int error = 0;
int lastTemp =0;
long integralTimeBuffer =0;

int PIDControl(int setPoint, int P, int I, int D){
    error = setPoint - readTemp;
    if (millis() - integralTimeBuffer >= 1000) {
        acc += error;
        derivate = readTemp - lastTemp;
        lastTemp = readTemp;
        integralTimeBuffer = millis();
    }    

    POWER = error * P + acc * I + derivate * D;
    return POWER;
}

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
    if (readTemp != memTemp)
    {
        lcd.setCursor(12,0);
        lcd.print("     ");
        lcd.setCursor(12,0);
	    lcd.print(readTemp);
        memTemp = readTemp;
    }    
    
}    

int heatRate = 10;//[]
int reflowTemp = 220;
int peakTemp = 250;
int reflowTime = 60;



void setup() {

    Wire.begin();
    Wire.beginTransmission(0x27);
    lcd.begin(16, 2);
    
    lcd.clear();
    lastTemp = readTemp;
    lcd.setCursor(0,0);

    lcd.setBacklight(255);
    lcd.print("Temperature:      C");

    pinMode(9, OUTPUT);
    delay(500);
    Serial.begin(9600);
}

void loop() {
    getTemp();
    printTemp();
    
    digitalWrite(9,(millis()/1000)%2);

}

