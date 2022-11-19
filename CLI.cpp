
#include "CLI.h"
#include <SoftwareSerial.h>
#include "ServoTimer2.h"  // the servo library
#include "VI_Measurement.h"
#include "utility.h"

extern SoftwareSerial BTSerial;

extern float SolarCell_V;
extern float SolarCell_mA;
extern float Charge_V;
extern float Charge_mA;
extern float Discharge_V;
extern float Discharge_mA;

extern char Version[];
extern char VersionDate[];

extern ServoTimer2 servo_1;
extern byte servo_1_Pin;
extern int Servo_1_uS;
extern byte servo_1_pwrPin;

const int Min_us = 1000;
const int Max_us = 2000;

extern byte LEDpin2;

// Command Line Interpreter - Global Variables
char CLI_Msg[50];
int CLI_i = 0;

// Collect the characters into a command string until end end of line,
// and then process it.
// V1.0 22/12/2015
void CLI_Process_Message(void)
{
	// Accumulate characters in a command string up to a CR or LF or buffer fills. 
	while (BTSerial.available())
	{
		char received = BTSerial.read();
		CLI_Msg[CLI_i++] = received;

		// Process message when new line character is received
		if (received == '\n' || received == '\r' || CLI_i == (sizeof(CLI_Msg) - 3))
		{
			CLI_Msg[CLI_i] = '\0';
			CLI_Processor();
			CLI_i = 0;
		}
	}
}

// Process the Command String.
// Split into command and parameters separated by commas. 
// V1.0 22/12/2015
void CLI_Processor()
{
	Serial.println(CLI_Msg);
	
	char cmd[4] = "";
	char param1[12] = "";
	char param2[12] = "";

	strcat(CLI_Msg, ",,");

	// Split into command and parameters separated by commas. 
	strncpy(cmd, strtok(CLI_Msg, ","), sizeof(cmd) - 1);
	strncpy(param1, strtok(NULL, ","), sizeof(param1) - 1);
	strncpy(param2, strtok(NULL, ","), sizeof(param2) - 1);


	// ===============================================
	// Command ech: Echo the command and parameters
	// ===============================================
	if (!strncmp(cmd, "ech", 3))
	{
		delay(50);
		BTSerial.print(cmd);
		BTSerial.print(",");
		BTSerial.print(param1);
		BTSerial.print(",");
		BTSerial.print(param2);
		BTSerial.println();
	}

	// ===============================================
	// Command ver, Get Software Version
	// ===============================================
	// No parameters
	if (!strncmp(cmd, "ver", 3))
	{
		delay(50);
		BTSerial.print(cmd);
		BTSerial.print(",");
		BTSerial.print(Version);
		BTSerial.print(",");
		BTSerial.print(VersionDate);
		BTSerial.println();

		//FlashLed2(2);
	}

	// ===============================================
	// Command gvi, Get voltage and current measurement
	// ===============================================
	// No parameters
	if (!strncmp(cmd, "pow", 3) )
	{
		char FloatFormatString[16];
		read_ina3221();

		BTSerial.print(cmd);
		BTSerial.print(",");
		BTSerial.print(dtostrf(SolarCell_V, 7, 3, FloatFormatString));
		BTSerial.print(",");
		BTSerial.print(dtostrf(SolarCell_mA, 7, 2, FloatFormatString));

		BTSerial.print(",");
		BTSerial.print(dtostrf(Charge_V, 7, 3, FloatFormatString));
		BTSerial.print(",");
		BTSerial.print(dtostrf(Charge_mA, 7, 2, FloatFormatString));

		BTSerial.print(",");
		BTSerial.print(dtostrf(Discharge_V, 7, 3, FloatFormatString));
		BTSerial.print(",");
		BTSerial.print(dtostrf(Discharge_mA, 7, 2, FloatFormatString));
		BTSerial.println();
	}

	// ===============================================
	// Command srv, Set servo position uS.
	// ===============================================
	// Parameter 1: Servo Number
	// Parameter 2: Servo Position uS

	if (!strncmp(cmd, "srv", 3))
	{
		Servo_1_uS = atoi(param1);  // Servo Position uS

		if ((Min_us < Servo_1_uS) && (Servo_1_uS < Max_us))
		{
			// green LED on
			digitalWrite(LEDpin2, HIGH);

			// Power on Servo 1
			pinMode(servo_1_pwrPin, OUTPUT);
			digitalWrite(servo_1_pwrPin, LOW);

			delay(200);

			// attach the servo, move, detach, ensure signal pin is low
			servo_1.attach(servo_1_Pin);

			servo_1.write(Servo_1_uS);
			delay(300);  // was 250, changed to 300, now 400, then 600, now 300
			servo_1.detach();

			// set the signal to be an output and LOW.
			pinMode(servo_1_Pin, OUTPUT);
			digitalWrite(servo_1_Pin, LOW);

			// power off Servo 1
			pinMode(servo_1_pwrPin, OUTPUT);
			digitalWrite(servo_1_pwrPin, HIGH);

			BTSerial.print(cmd);
			BTSerial.print(",");
			BTSerial.print(param1);
			BTSerial.println();

			// green LED off
			digitalWrite(LEDpin2, LOW);
		}
	}

	// Get Servo value
	if (!strncmp(cmd, "gsv", 3))
	{
		// send response 
		BTSerial.print(cmd);
		BTSerial.print(",");
		BTSerial.print(Servo_1_uS);
		BTSerial.println();
	}

	// Flash Red LED n flashes
	if (!strncmp(cmd, "flr", 3))
	{
		FlashLed(atoi(param1));  
	}

	// Flash Green LED n flashes
	if (!strncmp(cmd, "flg", 3))
	{
		FlashLed2(atoi(param1));
	}

}