/*
 * Hackerling_Shield.cpp
 *
 *  Created on: Dec 24, 2012
 *      Author: garratt
 */



#include <Wire.h>
#include <avr/pgmspace.h>
#include "MCP23017.h"
#include "Hackerling_Shield.h"


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif



SIGNAL(ADC_vect){
  hs.analogReadCallback();
}





void Speaker::playNote(uint8_t n,uint16_t dur){
	setNote(n);
	turnOn();
	delay(dur);
	turnOff();
	delay(5);
}


















	//Bargraph class for controlling leds, behaviors
	//IR Comm class
	// - receiver can receive raw data, like from a tv remote
	//   transmitter can send raw data, like tv remote
	//   can be set up to send data to other boards
	//Pot class
	//Buttons class
	//dipswitch class
	//servo class
	//speaker class - depends on tone class
	//analog IO class
	//thermistor class
	//LCD class


//button functions:
void Hackerling_Shield::initButtons(){
	//Buttons:
	//buttons are laid out in up-down-left-right arrow configuration
	//set 4 lines to input:
	//Button 1 - Left - GPB0
	//Button 2 - Up - GPA6
	//Button 3 - Right - GPA5
	//Button 4 - Down - GPA7
	hs_button_masks[0] = 0x0100;
	hs_button_masks[1] = 0x0040;
	hs_button_masks[2] = 0x0020;
	hs_button_masks[3] = 0x0080;
	//use MCP2017 functions to init state.
	mcp23017.pinMode(8,INPUT);
	mcp23017.pinMode(6,INPUT);
	mcp23017.pinMode(5,INPUT);
	mcp23017.pinMode(7,INPUT);
	mcp23017.pullUp(8,1);
	mcp23017.pullUp(6,1);
	mcp23017.pullUp(5,1);
	mcp23017.pullUp(7,1);

}
//give back the Buttons that are activated
uint8_t Hackerling_Shield::readButtons(){
	uint8_t ret=0;
	uint16_t bvalues = mcp23017.readGPIOAB();
	for(int i=0;i<4;i++)
		if( (bvalues & hs_button_masks[i]) == 0 ) //if value is low, then button is depressed
			ret |= (1<<i);
	return ret;
}

//Switch functions:
void Hackerling_Shield::initSwitches(){

	//use MCP2008 functions to init state.
	//dip switches: (numbered right to left)
	// 1-3: GP5-7
	// 4:   backlight, pin D8/PB0
	mcp23008.pinMode(5,INPUT);
	mcp23008.pinMode(6,INPUT);
	mcp23008.pinMode(7,INPUT);
	mcp23008.pullUp(6,1);
	mcp23008.pullUp(5,1);
	mcp23008.pullUp(7,1);
	DDRB &= 0xfe;
	PORTB |= 0x01;
	//the big toggle switch is connected to D4:
	DDRD &= 0xef;
	PORTD |= 0x10;

}

void Hackerling_Shield::initBarGraph(){
	//GP0  - Bargraph0
	//GP1  - Bargraph1
	//GP2  - Bargraph2
	//GP3  - Bargraph3
	//GP4  - Bargraph4
	//GPA0 - Bargraph5
	//GPA1 - Bargraph6
	//GPA2 - Bargraph7
	//GPA3 - Bargraph8
	//GPA4 - Bargraph9
	//since we need to activate pins 0-4 on both chips, the code is easy:
	for(int i=0;i<5;i++){
		mcp23008.pinMode(i, 1);// set pin to output
		mcp23017.pinMode(i, 1);// set pin to output
		mcp23008.digitalWrite(i, 1);// set pin to high (turns LED off)
		mcp23017.digitalWrite(i, 1);// set pin to high (turns LED off)
	}
}

void Hackerling_Shield::setBarGraph(uint16_t bgstate){
	uint16_t b = 0x0001, b1 = 0x0020;
	for(int i=0;i<5;i++){
		if(b & bgstate)
			mcp23008.digitalWrite(i, 0);// set pin to low (turns LED on)
		else
			mcp23008.digitalWrite(i, 1);// set pin to high (turns LED off)
		if(b1 & bgstate)
			mcp23017.digitalWrite(i, 0);// set pin to low (turns LED on)
		else
			mcp23017.digitalWrite(i, 1);// set pin to high (turns LED off)
		b = b<<1;
		b1 = b1<<1;
	}

}

//give back the Switches that are activated
uint8_t Hackerling_Shield::readSwitches(){
	//dip0 - PB0
	//dip1 - GP7
	//dip2 - GP6
	//dip3 - GP5
	uint8_t readvalue = mcp23008.readGPIO();
	uint8_t svalues=0;
	svalues |= (PINB & 0x01);  //dip 0
	svalues |= (readvalue >> 6) & 0x02; //dip 1
	svalues |= (readvalue >> 4) & 0x04; //dip 2
	svalues |= (readvalue >> 2) & 0x08; //dip 3
	svalues = svalues ^ 0x0f;
	svalues |= (PIND & 0x10);
	return svalues;
}


//analog functions:
void Hackerling_Shield::initAnalog(){
	// init analog inputs:
	//Pot: A3 -
	//thermistor: A2
	//analog input: A0, A1
//set the ADC to run in free-running mode
	//prescaler: 128 - so the ADC clock would be 125Khz
	//conversion takes 13 clock cycles, we have 5 channels (we'll read internal temp), so a 5 channel read will take 65 ADC cycles.
	//That is still updating at 1.9Khz, which will be fine, unless we are trying to use the analog in as an oscilloscope.
	//ADCSRA
	//  1xxx xxxx   ADC enable
	//  x0xx xxxx   Don't start conversion right now
	//  xx0x xxxx   Auto trigger conversions
	//  xxx0 xxxx   Interrupt flag
	//  xxxx 1xxx   ADC interrupt enable
	//  xxxx x111   ADC prescaler - 128
	ADCSRA = 0x8f;
	//ADCSRB
	//  0xxx xxxx   reserved
	//  x0xx xxxx   Analog Comparator Multiplexer Enable (disabled)
	//  xx00 0xxx   reserved
	//  xxxx x000   Free Running mode
	ADCSRB = 0x00;
	DIDR0 = 0x0c;  //disable digital input for pot and thermistor.
	ADMUX = 0x68;  //ref: Avcc, Left align (so we can just read 8 bits), first conversion is temperature
	analog_channels = 0x010f;//the 8 bit is the internal temperature adc
	ADCSRA |= 0x40; //start conversion
}

//this function should be called by the ADC conversion complete interrupt
void Hackerling_Shield::analogReadCallback(){
	uint8_t curr_channel = ADMUX & 0x0f;
	uint16_t curr_channel_mask = 1 << curr_channel;
	//read value:
	analog_values[curr_channel] = ADCH;
	//find next channel:
	curr_channel_mask = (curr_channel_mask <<1) & 0x1ff;
	curr_channel++;
	//shift channel until we find one that we want to read
	while((curr_channel_mask & analog_channels)==0){
		curr_channel= (curr_channel+1)%9;
		if(curr_channel_mask & 0x1ff)
			curr_channel_mask = (curr_channel_mask <<1) & 0x1ff; //shift to the next channel
		else
			curr_channel_mask=1;
	}
	ADMUX &= 0xf0;
	ADMUX |= curr_channel; //don't need to mod by 0x0f, because it will never be above 9
	ADCSRA |= 0x40; //start conversion
}

uint8_t Hackerling_Shield::getPotValue(){
	return analog_values[3];
}
uint8_t Hackerling_Shield::getThermistorValue(){
	return analog_values[2];
}
uint8_t Hackerling_Shield::getInternalTempValue(){
	return analog_values[8];
}
void Hackerling_Shield::begin(){
	//Dependents:
	//Serial
	  //io-expanders
	mcp23017.begin(0);
	mcp23008.begin(1);
	initSwitches();
	initButtons();
	initBarGraph();
	initAnalog();
	speaker.begin();
	   // Buttons
	   // Bar Graph
	   // dip switch

}



// Preinstantiate Objects //////////////////////////////////////////////////////

Hackerling_Shield hs = Hackerling_Shield();


