
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
	Serial.print("Step 21 Verification:\n Toggle the DIP switchs to verify that each works.\n");
}

#define TEST_STATE_SWITCH1_ON 1
#define TEST_STATE_SWITCH1_OFF 0x10
#define TEST_STATE_SWITCH2_ON 2
#define TEST_STATE_SWITCH2_OFF 0x20
#define TEST_STATE_SWITCH3_ON 0x04
#define TEST_STATE_SWITCH3_OFF 0x40
#define TEST_STATE_SWITCH4_ON 0x08
#define TEST_STATE_SWITCH4_OFF 0x80
#define TEST_PASS 0xff

void printStates(uint8_t b){
	uint8_t comp=1;
	for(int i=0;i<4;i++){ //for each of the dips
		Serial.print("| ");
		if(b & comp)
			Serial.print("On ");
		else
			Serial.print("   ");
		if(b & (comp<<4))
			Serial.print("Off ");
		else
			Serial.print("    ");
		Serial.print(" | ");
		comp = comp<<1;
	}

}

uint8_t response=0;
uint8_t state=0;
uint8_t total=0;
void loop() {
	state=0;
	response = hs.readSwitches();

	state=(response & 0x0f) | ((response ^ 0xff) << 4);


	total |= state;
	if(total){
		Serial.print("Current:   ");
		printStates(state);
		Serial.print("  Seen:   ");
		printStates(total);
		if(total == TEST_PASS)
			Serial.print("      Test Complete: PASS! ");
		Serial.print("\n");
	}
	delay(100);

}
