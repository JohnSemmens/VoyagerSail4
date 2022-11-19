/*
	Name:       VoyagerSail4.ino
	Author:     ACS\john
*/

// VoyagerSail V4
// This is the version 4 Sail controller It revert back to Bluetooth as BT5 to replace the LoRa.


// Commands: 
//	ver - return software version details
//  pow - return power supply values: Solar Voltage, Solar Current, Battery Voltage, Charge Current, Discharge Voltage,  Battery Voltage(again), Load Current.
//  gsv - Get Servo position (in microseconds)
//  ssv - Set servo position (in microseconds)
//  flr - Flash Red LED n times
//  flg - Flash Green LED n times

// V4.01 7/5/2022 Reverting back to Bluetooth as BT5. Commence development
// V4.02 11/5/2022 added servo power switching on the "srv" command
// V4.03 14/5/2022 added LED Flash procedures to Utilities
// V4.04 18/5/2022 add a read of the MAC address and send to serial port. This to aid data gathering and setting config on Voyager.
// V4.05 20/5/2022 activate thr green LED with the SRV command to help diagnose behaviour.

char Version[] = "Voyager Sail V4.05"; // Voyager Sail BT5 Servo Controller 
char VersionDate[] = "20/5/2022";

#include "VI_Measurement.h"
#include "utility.h"
#include "CLI.h"
#include <SoftwareSerial.h>
#include <Wire.h>
#include "ServoTimer2.h"  // the servo library

#include <avr/sleep.h>
#include <avr/wdt.h>
#include <avr/power.h>

byte LEDpin = 13; //PB5 Red
byte LEDpin2 = 5; //PD5 Green

byte servo_1_Pin = 3; //PD3
byte servo_1_pwrPin = 6; //PD6
int Servo_1_FailSafe_uS = 1500; // micro seconds
int Servo_1_uS;
ServoTimer2 servo_1;

// Define BT Pins
byte RxPin = PD2; //9;
byte TxPin = 8;
byte StatePin = 10;
SoftwareSerial BTSerial(RxPin, TxPin);
bool BTConnectedState = false;
bool PrevBTConnectedState = false;

unsigned long SleepMillis;

#define Ch1 1
#define Ch2 2
#define Ch3 3

float SolarCell_V;
float SolarCell_mA;
float Charge_V;
float Charge_mA;
float Discharge_V;
float Discharge_mA;

char c;

// watchdog interrupt
ISR(WDT_vect)
{
	wdt_disable();  // disable watchdog and wakeup
}

void wakeUp()
{
	// stub for the interrupt vector for the external interrupt for EByte_AuxPin
}

void setup()
{
	// set up LED
	pinMode(LEDpin, OUTPUT);
	pinMode(LEDpin2, OUTPUT);
	FlashBothLeds(2);

	// Set up Serial Command Port
	Serial.begin(9600);
	BTSerial.begin(9600);
	delay(300);

	// setup  to read status line from BT Module.
	pinMode(StatePin, INPUT);

	// if we restart, and the Bluetooth is already connected, then don't bother with the AT commands.
	// because they will simply pass through, and be ignored.
	// Bluetooth connection not established =  Low
	if (digitalRead(StatePin) == LOW)
	{
		// Setup Bluetooth slave module
		BTSerial.println("AT+DEFAULT");
		Serial.println("AT+DEFAULT");
		delay(2000);

		BTSerial.println("AT+ROLE0");
		Serial.println("AT+ROLE0");
		delay(10);

		BTSerial.println("AT+NAMEVoyagerSail");
		Serial.println("AT+NAMEVoyagerSail");
		delay(10);

		BTSerial.println("AT+RESET");
		Serial.println("AT+RESET");
		delay(1000);
	}

	// Setup Servo Control
	// set the signal to be an output and LOW.
	pinMode(servo_1_Pin, OUTPUT);
	digitalWrite(servo_1_Pin, LOW);

	// Setup Power control on Servo 1
	pinMode(servo_1_pwrPin, OUTPUT);
	digitalWrite(servo_1_pwrPin, HIGH);// servo power off

	// start the 3 channel V/I measurement
	init_ina3221();

	// wiggle servo on start up.
	wiggleServo();

	// set up an interrupt for wake on Aux signal from LoRa Module
	// Aux pin is pin 8. This works... which is good. But interrupts are supposed to only work on Arduino pins 2 and 3. So why does this work ?
	attachInterrupt(digitalPinToInterrupt(PD2), wakeUp, CHANGE);

	SleepMillis = millis() + (1000 * 5); // set the sleeping time to 20  seconds in the future. i.e. stay awake for 2s 
	digitalWrite(LEDpin, HIGH); // awake now
}

void loop()
{
	// maintain previous state for transition detection
	PrevBTConnectedState = BTConnectedState;

	// Bluetooth connection established =  High
	if (digitalRead(StatePin) == HIGH)
	{
		BTConnectedState = true;
		digitalWrite(LEDpin, HIGH);
		// wait for serial command within command processor
		CLI_Process_Message();
	}
	else
	{
		BTConnectedState = false;
		FlashLed(2);
		delay(2000);
	};

	// detect a transition to a connected state.
	// show a signal of some type
	if (PrevBTConnectedState == false && BTConnectedState == true)
	{
		FlashLed(10);
	}


	if (millis() > SleepMillis) // stay awake until we get the sleeping time.
		// go to sleep
	{
		// disable ADC
		ADCSRA = 0;

		// clear various "reset" flags
		MCUSR = 0;
		// allow changes, disable reset
	//	WDTCSR = bit(WDCE) | bit(WDE);
		// set interrupt mode and an interval
	//	WDTCSR = bit(WDIE) | bit(WDP3) | bit(WDP0);    // set WDIE, and 8 seconds delay
		//WDTCSR = bit(WDIE) | bit(WDP2) | bit(WDP1) ;    // set WDIE, and 2 seconds delay
	//	wdt_reset();  // pat the dog

		set_sleep_mode(SLEEP_MODE_PWR_DOWN); // seems to stop it working
		noInterrupts();           // timed sequence follows
		sleep_enable();

		// turn off brown-out enable in software
		MCUCR = bit(BODS) | bit(BODSE);
		MCUCR = bit(BODS);


		digitalWrite(LEDpin, LOW); // LED off, go to sleep
		interrupts();             // guarantees next instruction executed
		sleep_cpu(); //	<== Sleep here 
		//
		//*********************************************
		// wake on interrupt from EByte_AuxPin
		//				<== Wake up here.
		digitalWrite(LEDpin, HIGH); // awake now
		// cancel sleep as a precaution
		sleep_disable();

		SleepMillis = millis() + (1000 * 1);  //set the sleeping time to 1 seconds in the future. i.e.stay awake for 1s
	}

}
