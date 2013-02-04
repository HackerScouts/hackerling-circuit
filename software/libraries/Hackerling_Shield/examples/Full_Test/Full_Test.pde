#include <IR_COM.h>

#include <LCD.h>

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
//Hackerling_Shield hs;


uint16_t pin=3;

uint8_t pot;
uint8_t fullpot;
uint8_t fullswitch;
uint8_t fulldips;
uint8_t buttons=0;
uint8_t total=0;

void setup(){
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
	hs.begin(NO_LCD);
	fullpot=0;
	fullswitch=0;
	fulldips=0;
}

uint8_t up=1;
int8_t updateGraph(){
	hs.setBarGraph(pin);
	if(up)
		pin = pin <<1;
	else
		pin = pin >>1;
//	if it goes off the edge:
	if(pin >= 0x0c00){
		pin=0x0300;
		up=0;
		return -1;
	}
	if(pin<=0){
		pin=0x0003;
		up=1;
		return 1;
	}
	return 0;
}

void playButtonNotes(uint8_t b){
	uint8_t scale=pot/5+2;
		if(fulldips==0xff)
			hs.speaker.playNote(NOTE_B,2*scale);
		if(fullswitch==3)
			hs.speaker.playNote(NOTE_C,2*scale);
		if(b & BUTTON_UP)
			hs.speaker.playNote(NOTE_D,2*scale);
		if(b & BUTTON_LEFT)
			hs.speaker.playNote(NOTE_E,2*scale);
		if(b & BUTTON_RIGHT)
			hs.speaker.playNote(NOTE_F,2*scale);
		if(b & BUTTON_DOWN)
			hs.speaker.playNote(NOTE_G,2*scale);
		if(fullpot==3)
			hs.speaker.playNote(NOTE_A,2*scale);
		delay(100);
}

void verifiers(){
	//verifiers
	if(pot==0){
		fullpot |= 0x01;
	}
	if(pot==255){
		fullpot |= 0x02;
	}


	uint8_t response = hs.readSwitches();
	hs.speaker.setOctave((response & 0x03)+1);
	if((response & 0x10) ==0){
		fullswitch |= 0x01;
	}
	if((response & 0x10) == 0x10){
		fullswitch |= 0x02;
	}
	uint8_t dips=(response & 0x0f) | ((response ^ 0xff) << 4);
	fulldips |= dips;

}

void errors(){

	if(fulldips!=0xff)
		Serial.print("Dips ");
	if(fullpot!=3)
		Serial.print("Pot ");
	if(fullswitch!=3)
		Serial.print("Switch ");
	if(total!=0x0f)
		Serial.print("Buttons ");
	Serial.print(int(pot));Serial.print("\n");

}


void loop() {
        //TODO: make done condition where it behaves differently
	buttons = hs.readButtons();
	verifiers();
	pot = hs.getPotValue();
	total |= buttons;
	if(updateGraph())
		playButtonNotes(total);
	errors();
	DelayLoop(pot/5);
//	Serial.print("Pin "); Serial.print(pin);Serial.print("\n");


}
