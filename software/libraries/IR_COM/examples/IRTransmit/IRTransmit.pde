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
#include <IR_COM.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
//MCP23017 iomap = MCP23017();

IR_COM IR;

void setup() {
  // Debugging output
  Serial.begin(9600);
  IR.begin();
        IR.rxflush();
  // set up the LCD's number of columns and rows:
//	iomap.begin(0);
//	iomap.pinMode(BARGRAPH5, OUTPUT);
//	iomap.pinMode(BARGRAPH6, OUTPUT);
//	iomap.pinMode(BARGRAPH7, OUTPUT);
//	iomap.pinMode(BARGRAPH8, OUTPUT);
//	iomap.pinMode(BARGRAPH9, OUTPUT);
//	//set all the pins to high (turns LEDs off)
//    iomap.digitalWrite(BARGRAPH5,1);
//    iomap.digitalWrite(BARGRAPH6,1);
//    iomap.digitalWrite(BARGRAPH7,1);
//    iomap.digitalWrite(BARGRAPH8,1);
//    iomap.digitalWrite(BARGRAPH9,1);

  //      DDRD  |= 0x04;
}

        Circular_Buffer circ;
void mydelay_ms(uint16_t _delay){
	IR.resetCount();
	while(IR.countcheck(_delay));
}

uint8_t pin=0;
void loop() {
   //set current pin to 0:
//	iomap.digitalWrite(pin,1);
//    pin=(pin+1)%5; //loop from 0 to 4
//	iomap.digitalWrite(pin,0);

        IR.rxflush();
        IR.txflush();
//	for(int i=0;i<5;i++)
		IR.write(0x0a);
	        mydelay_ms(50);
		IR.write(0xa0);
      //  }
	mydelay_ms(500);
//	mydelay_ms(500);
//	mydelay_ms(500);

		Serial.print(IR.rx.transitions);
		Serial.print(" ");
		Serial.print(IR.rx.errors);
		Serial.print(" ");
for(int i=0;i<12;i++){
		Serial.print(" (");
		Serial.print(IR.rx.lengths[i]);
		Serial.print(", ");
		Serial.print(IR.rx.values[i]);
		Serial.print(")");
}
Serial.print(" ->   ");
	int in = IR.read();
        unsigned char count='g';
	while(in > 0 && count<'r'){
		unsigned char c= in;
		Serial.print((int)IR.available());
		Serial.print((int)in);
		Serial.print(" ");
	        in = IR.read();
                count++;
	}
circ.push('h');
circ.push('e');
circ.push('y');

Serial.print((char)circ.pop());
Serial.print((char)circ.pop());
Serial.print((char)circ.pop());
	Serial.print(" in \n");

}
