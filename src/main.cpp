#include <Arduino.h>

#include <max6675.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

int thermoDO = 4;
int thermoCS = 5;
int thermoCLK = 6;

MAX6675 T0(thermoCLK, thermoCS, thermoDO);
int readTemp;
int KP = 1;
int kI = 1000;
int KD = 0;

long lastSampleTime=0;
long acc = 0;
long derivate =0;

void getTemp(){
    readTemp = T0.readCelsius()*10;
    lastSampleTime = millis();
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

LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

#define HEAT_PIN 9
#define COOL_PIN 8

#define ENCA_PIN
#define ENCB_PIN
#define ENCP_PIN


void setup() {
    lcd.backlight();
    lastTemp = readTemp;
    lcd.setCursor(0,0);
    lcd.backlight();
    lcd.print("Temperature:      C");
}

void loop() {
    getTemp();
    lcd.setCursor(12,0);
	lcd.print(readTemp);
}

