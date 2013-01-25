#include <IR_COM.h>




/*
 * IRTransmit.pde
 *
 *  Created on: Dec 26, 2012
 *      Author: garratt
 */




/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
//MCP23017 iomap = MCP23017();
void toggleLED1a(){
  if(PORTB & 0x04)
    PORTB &= 0xfb;
  else
    PORTB |= 0x04;
}

void toggleLED2a(){
  if(PORTB & 0x02)
    PORTB &= 0xfd;
  else
    PORTB |= 0x02;
}

IR_COM IR;
void setup() {
  // Debugging output
  Serial.begin(19200);
  IR.begin();
        IR.txflush();

  DDRB |=0x06; //enable LEDs
}

void mydelay_ms(uint16_t _delay){
	IR.resetCount();
	while(IR.countcheck(_delay));
}


uint8_t pin=0;
void loop() {
   //     IR.rxflush();
  //      IR.txflush();
		IR.write(0x00);
		IR.write(0xF7);
		IR.write(0xC0);
		IR.write(0x3F);
	mydelay_ms(1000);
	mydelay_ms(1000);
	mydelay_ms(1000);
		Serial.println("transmitted");

}
