// 
// 
// 

#include "utility.h"
#include "ServoTimer2.h"  // the servo library

extern byte LEDpin;
extern byte LEDpin2;

extern int Servo_1_FailSafe_uS;
extern int Servo_1_uS;

extern ServoTimer2 servo_1;
extern byte servo_1_Pin;
extern byte servo_1_pwrPin;

void wiggleServo(void)
{
	// wiggle the servo
	// this is useful for a power-on signal and dignostics
	// v1.0 29/9/2018

	int wiggleAmount = 200; //microseconds

	// Power on Servo 1
	pinMode(servo_1_pwrPin, OUTPUT);
	digitalWrite(servo_1_pwrPin, LOW);
	delay(200);

	servo_1.attach(servo_1_Pin);
	servo_1.write(Servo_1_FailSafe_uS - wiggleAmount);
	delay(200);
	servo_1.write(Servo_1_FailSafe_uS + wiggleAmount);
	delay(200);
	servo_1.write(Servo_1_FailSafe_uS);
	delay(200);
	servo_1.detach();

	// set the signal to be an output and LOW.
	pinMode(servo_1_Pin, OUTPUT);
	digitalWrite(servo_1_Pin, LOW);

	// power off Servo 1
	pinMode(servo_1_pwrPin, OUTPUT);
	digitalWrite(servo_1_pwrPin, HIGH);

	Servo_1_uS = Servo_1_FailSafe_uS;
}

void FlashLed(int FlashCount)
{
	for (int i = 0; i < FlashCount; i++)
	{
		digitalWrite(LEDpin, HIGH);
		delay(50);
		digitalWrite(LEDpin, LOW);
		delay(100);
	}
}

void FlashLed2(int FlashCount)
{
	for (int i = 0; i < FlashCount; i++)
	{
		digitalWrite(LEDpin2, HIGH);
		delay(50);
		digitalWrite(LEDpin2, LOW);
		delay(100);
	}
}

void FlashBothLeds(int FlashCount)
{
	for (int i = 0; i < FlashCount; i++)
	{
		digitalWrite(LEDpin, HIGH);
		digitalWrite(LEDpin2, HIGH);
		delay(50);
		digitalWrite(LEDpin, LOW);
		digitalWrite(LEDpin2, LOW);
		delay(100);
	}
}