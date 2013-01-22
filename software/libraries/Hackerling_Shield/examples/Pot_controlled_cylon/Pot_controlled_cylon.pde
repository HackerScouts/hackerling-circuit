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

}

uint16_t pin=3;
uint8_t up=1;
void loop() {
	hs.setBarGraph(pin);
	//move to next pin...

	if(up)
		pin = pin <<1;
	else
		pin = pin >>1;

//	if it goes off the edge:
	if(pin >= 0x0c00){
		pin=0x0300;
		up=0;
	}
	if(pin<=0){
		pin=0x0003;
		up=1;

	}

uint8_t pot = hs.getPotValue();
	Serial.print("Pot: "); Serial.print(int(pot));Serial.print("\n");
	delay(pot);

}
