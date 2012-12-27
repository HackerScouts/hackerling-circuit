/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <MCP23008.h>
#include <MCP23017.h>
#include <Hackerling_Shield.h>
#include "Print.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
MCP23008 iomap = MCP23008();



void setup() {
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
	iomap.begin(1);
	iomap.pinMode(BARGRAPH0, OUTPUT);
	iomap.pinMode(BARGRAPH1, OUTPUT);
	iomap.pinMode(BARGRAPH2, OUTPUT);
	iomap.pinMode(BARGRAPH3, OUTPUT);
	iomap.pinMode(BARGRAPH4, OUTPUT);
	//set all the pins to high (turns LEDs off)
    iomap.digitalWrite(BARGRAPH5,1);
    iomap.digitalWrite(BARGRAPH6,1);
    iomap.digitalWrite(BARGRAPH7,1);
    iomap.digitalWrite(BARGRAPH8,1);
    iomap.digitalWrite(BARGRAPH9,1);

}

uint8_t pin=0;
void loop() {
   //set current pin to 0:
	iomap.digitalWrite(pin,1);
    pin=(pin+1)%6; //loop from 0 to 4
	iomap.digitalWrite(pin,0);
	Serial.print("Pin "); Serial.print(pin);
	delay(100);

}
