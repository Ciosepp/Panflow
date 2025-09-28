/*
	Timer library based on micros() function.
	Written by G. Balducci, March 15, 2022.
	Released into the public domain.
	Version 2.0
*/

#ifndef	Timer_h
#define Timer_h

#include "Arduino.h"

enum scaling_t {MILLIS, MICROS};

class Timer{

	public:
		Timer(scaling_t Scaling = MILLIS);
		~Timer();

		void startDelay();
		void setDelayTime(uint32_t time);
		bool getDelayQ();
		uint32_t getRemainingTime();

		void TriggMonoTimer(uint32_t dTime);
		bool getMonoQ();

		void setClock(long period, int dt);
		bool getClock();

		bool timerClock(uint32_t t);

	private:
		scaling_t scaling;
		//uint32_t scaling;

		uint32_t CurrTime();
		uint32_t t0;
		uint32_t tDelay;

		uint32_t tMono0;
		uint32_t tMonoDelay;
		bool monoQ = false;

		uint32_t clockBuffer;
		uint32_t tHigh;
		uint32_t tLow;
		bool CK = true;

		uint32_t tCK = 0;
		bool tCK_state = true;
	
	};
#endif