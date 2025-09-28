#include <Arduino.h>
#include "Timer.h"

Timer::Timer(scaling_t Scaling){
		this ->scaling = Scaling;
	}

Timer::~Timer(){};

//returns millis or micros value acconding to glo scaling
uint32_t Timer::CurrTime(){		
	if(scaling == MILLIS) return millis();
	else return micros();
}

//records t0 as start instant
void Timer::startDelay(){

	t0 = CurrTime();
}

///...
void Timer::setDelayTime(uint32_t Dtime){
	tDelay = Dtime;
}

// if current time is >= of elapsed time retuens true
bool Timer::getDelayQ(){

	if( CurrTime() >= (t0+tDelay) ) return true;
	
	else
		return false;
}

//....
uint32_t Timer::getRemainingTime(){

	return t0 +  tDelay- CurrTime();
}

void Timer::TriggMonoTimer(uint32_t dTime){

	tMono0 = CurrTime();
	tMonoDelay = dTime;
	monoQ = true;
}


bool Timer::getMonoQ(){

	if( (CurrTime() >= tMono0 + tMonoDelay) && monoQ){
		monoQ = false;
	}
	return monoQ;
}

void Timer::setClock(long period, int dt){
	
	tHigh = period*dt/100;
	tLow= period*(100-dt)/100;

	clockBuffer = CurrTime() + tHigh;
}

bool Timer::getClock(){

	if(CurrTime() >= clockBuffer){
		
		if(CK){
			
			CK = false;
			clockBuffer = CurrTime() + tLow;
		}

		else{
			
			CK = true;
			clockBuffer = CurrTime() + tHigh;
		}
	}
	return CK;
}

bool Timer::timerClock(uint32_t t){
	if (micros() >= tCK)
	{
		tCK = micros() + t;
		return true;
	}
	return false;
}