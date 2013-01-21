#include <Wire.h>

#include <MCP23017.h>

#include <Hackerling_Shield.h>

#include <MCP23008.h>

// include the library code:
//#include <Wire.h>
//#include <MCP23017.h>
#include "Print.h"

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.

void setup() {
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
	hs.begin();
	Serial.print("Press each button to verify that it works.\n");
}

void printButtons(uint8_t b){
	if(b & BUTTON_UP)
		Serial.print("Up ");
	else
		Serial.print("   ");
	if(b & BUTTON_LEFT)
		Serial.print("Left ");
	else
		Serial.print("     ");
	if(b & BUTTON_RIGHT)
		Serial.print("Right ");
	else
		Serial.print("      ");
	if(b & BUTTON_DOWN)
		Serial.print("Down ");
	else
		Serial.print("     ");
}

uint8_t buttons=0;
uint8_t total=0;
void loop() {

	buttons = hs.readButtons();
	total |= buttons;
	if(total){
		Serial.print("Current:   ");
		printButtons(buttons);
		Serial.print("  Seen:   ");
		printButtons(total);
		if(total == 0x0f)
			Serial.print("      Test Complete: PASS! ");
		Serial.print("\n");
	}
	delay(100);

}
