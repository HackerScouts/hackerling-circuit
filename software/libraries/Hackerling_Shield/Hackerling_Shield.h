/*
 * Hackerling_Shield.h
 *
 *  Created on: Dec 23, 2012
 *      Author: garratt
 */

#ifndef HACKERLING_SHIELD_H_
#define HACKERLING_SHIELD_H_
//Here is all the pin mapping for the hackerling shield:
#include <avr/io.h>
#include <MCP23017.h>
#include <MCP23008.h>
#include <LCD.h>
#include <IR_COM.h>

void DelayLoop(uint16_t l);

#define HCV1_1 1
#define HCV1_2 2
#define HC_VERSION 2

#define INPUT 0

//for disabling things at startup:
#define NO_LCD 0x01
#define NO_IR  0x02


//for reading the switches:
#define DIP1 0x01;
#define DIP2 0x02;
#define DIP3 0x04;
#define DIP4 0x08;
#define BIG_SWITCH 0x10;


//MCP23017 - 16 bit io mapper:
//GPA0 - Bargraph5
#define BARGRAPH5 0
//GPA1 - Bargraph6
#define BARGRAPH6 1
//GPA2 - Bargraph7
#define BARGRAPH7 2
//GPA3 - Bargraph8
#define BARGRAPH8 3
//GPA4 - Bargraph9
#define BARGRAPH9 4
//GPA5 - LCD_E
//GPA6 - LCD_R/W
//GPA7 - LCD_RS

//GPB0 - LCD_DB0
//GPB1 - LCD_DB1
//GPB2 - LCD_DB2
//GPB3 - LCD_DB3
//GPB4 - LCD_DB4
//GPB5 - LCD_DB5
//GPB6 - LCD_DB6
//GPB7 - LCD_DB7

//MCP23008 - 8 bit io mapper:
//GPA0 - Bargraph0
//GPA1 - Bargraph1
//GPA2 - Bargraph2
//GPA3 - Bargraph3
//GPA4 - Bargraph4
//GPA5 - DIP1
//GPA6 - DIP2
//GPA7 - DIP3
#define BARGRAPH0 0
#define BARGRAPH1 1
#define BARGRAPH2 2
#define BARGRAPH3 3
#define BARGRAPH4 4

		//Button 1 - Left - GPB0
		//Button 2 - Up - GPA6
		//Button 3 - Right - GPA5
		//Button 4 - Down - GPA7
#define BUTTON_LEFT  0x01
#define BUTTON_UP    0x02
#define BUTTON_RIGHT 0x04
#define BUTTON_DOWN  0x08

//The values to set the compare match to get specific tones.
//To change octave, we just change prescale
#define NOTE_B 253
#define NOTE_C 239
#define NOTE_Cs 225
#define NOTE_D 213
#define NOTE_Ds 201
#define NOTE_E 190
#define NOTE_F 179
#define NOTE_Fs 169
#define NOTE_G 159
#define NOTE_Gs 150
#define NOTE_A 142
#define NOTE_As 134
#define NOTE_Balt 127

//uses timer 2
class Speaker{
public:
	void turnOn(){
		TCCR2A &= 0xbf;
		TCCR2A |= 0x40;
	}
	void turnOff(){
		TCCR2A &= 0x3f;
	}
	void setOctave(uint8_t octave){
		//octaves that work well:
		//Octave 1: 123Hz to 233Hz - prescale 256 - 110
		//Octave 2: 246Hz to 466Hz - prescale 128 - 101
		//Octave 3: 493Hz to 932Hz - prescale 64 - 100
		//Octave 4: 987Hz to 1864Hz - prescale 32 - 011
		TCCR2B = (octave ^ 0x07) & 0x07; //just flip the bits!
		//note this assumes that we will always use ctc mode (decent assumption for this library)
		// also, if someone enters 7, it will stop the clock.
	}
	void setNote(uint8_t n){
		OCR2A=n;
	}
	void playNote(uint8_t n,uint16_t dur);

	void begin(){
		//initialize pins:
		DDRB |= 0x08;

		//start oscillator
		//TCCR2A
		// 0000xxxx - initially, do not toggle outputs on compare match
		// xxxx00xx - reserved
		// xxxxxx10 - CTC mode
		TCCR2A = 0x02;

		//TCCR2B
		// 00xxxxxx - Force output compare (don't)
		// xx00xxxx - reserved
		// xxxx0xxx - CTC mode
		// xxxxx011 - prescale at 32
//		TCCR2B = 0x03;
		setOctave(2);
//		set to play a C
		OCR2A = NOTE_C;


		//TIMSK0 - xxxxx010 - enable Compare match A interrupt
//		TIMSK0 = 0x02;
	}

};




class Hackerling_Shield{
private:
	//MCP23017 class - use directly only for installation tests
	MCP23017 mcp23017;
	//MCP23008 class - use directly only for installation tests
	MCP23008 mcp23008;
	uint16_t hs_button_masks[4];//the button mapping into the MCP23017 pins

public:
	//Bargraph class for controlling leds, behaviors
	//IR Comm class
	// - receiver can receive raw data, like from a tv remote
	//   transmitter can send raw data, like tv remote
	//   can be set up to send data to other boards
	//Pot class
	//Buttons class
	//dipswitch class
	//servo class

	//analog IO class
	//thermistor class
	//LCD class
	LCD lcd;

	IR_COM IR;

	Speaker speaker;

	//analog stuff:
	uint16_t analog_channels; //select which channels we will be reading
	uint8_t analog_values[9];//whenever a channel is read, put data in the appropriate bin.


	//button functions:
	void initButtons();
	//give back the Buttons that are activated
	uint8_t readButtons();

	//Switch functions:
	void initSwitches();

	void initBarGraph();

	void setBarGraph(uint16_t bgstate);

	//give back the Switches that are activated
	uint8_t readSwitches();


	//analog functions:
	void initAnalog();

	//this function should be called by the ADC conversion complete interrupt
	void analogReadCallback();

	uint8_t getPotValue();
	uint8_t getThermistorValue();
	uint8_t getInternalTempValue();
	void begin(uint8_t flags=0);

	//void IRCountCallback();




};



extern Hackerling_Shield hs;











#endif /* HACKERLING_SHIELD_H_ */
