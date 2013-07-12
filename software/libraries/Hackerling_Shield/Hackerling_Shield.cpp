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
#include "LCD.h"
#include <IR_COM.h>

//extern "C"{
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
//}
#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif



SIGNAL(ADC_vect){
  hs.analogReadCallback();
}

//TODO: These should work... investigate
//SIGNAL(TIMER0_COMPA_vect)
//{
//    hs.IR.callback();
//}
//
//
//
//SIGNAL(PCINT0_vect)
//{
//	hs.IR.rx.rxCallback();
//
//}



void DelayLoop(uint16_t l){
	hs.IR.resetCount();
	while(hs.IR.countcheck(l));
//
//	_delay_ms(l);
	//this is a horrible hack, but necessary to force compiler not to optimize it out...
//	for(uint16_t i=0;i<l;i++){
//		for(int)
//	  if(TIMSK0 & 0x01)
//	    TIMSK0 &= 0xfe;
//	  else
//	    TIMSK0 |= 0x01;
//	}
//	  //alternate:
//	 // TCCR1B & 0x80 - noise canceler
//	  _delay_loop_2(l);
}

void Speaker::playNote(uint8_t n,uint16_t dur){
	setNote(n);
	turnOn();
	DelayLoop(dur);
	turnOff();
	DelayLoop(5);
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
	//Button    Direction	V1.1		V1.2
	//Button 1 - 	Left  	GPB0 (8)	GPA7 
	//Button 2 - 	Up  	GPA6 (6)	GPA5
	//Button 3 - 	Right  	GPA5 (5)	GPA4
	//Button 4 - 	Down  	GPA7 (7)	GPA6
    #if HC_VERSION == HCV1_1	
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
    #elif HC_VERSION ==  HCV1_2
	hs_button_masks[0] = 0x0080;
	hs_button_masks[1] = 0x0020;
	hs_button_masks[2] = 0x0010;
	hs_button_masks[3] = 0x0040;
	//use MCP2017 functions to init state.
	mcp23017.pinMode(7,INPUT);
	mcp23017.pinMode(5,INPUT);
	mcp23017.pinMode(4,INPUT);
	mcp23017.pinMode(6,INPUT);
	mcp23017.pullUp(7,1);
	mcp23017.pullUp(5,1);
	mcp23017.pullUp(4,1);
	mcp23017.pullUp(6,1);
    #endif   


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
	
    #if HC_VERSION == HCV1_1	
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
    #elif HC_VERSION == HCV1_2
	mcp23017.pinMode(3,INPUT);
	mcp23017.pinMode(2,INPUT);
	mcp23017.pinMode(1,INPUT);
	mcp23017.pullUp(3,1);
	mcp23017.pullUp(2,1);
	mcp23017.pullUp(1,1);
	//the big toggle switch is connected to D4:
	DDRD &= 0xef;
	PORTD &= 0xef;
	
    #endif   

}

void Hackerling_Shield::initBarGraph(){
    #if HC_VERSION == HCV1_1	
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
    #elif HC_VERSION == HCV1_2
	//GPB0  - Bargraph0
	//GPB1  - Bargraph1
	//GP7  - Bargraph2
	//GP6  - Bargraph3
	//GP5  - Bargraph4
	//GP4 - Bargraph5
	//GP3 - Bargraph6
	//GP2 - Bargraph7
	//GP1 - Bargraph8
	//GP0 - Bargraph9
	for(int i=0;i<8;i++){
		mcp23008.pinMode(i, 1);// set pin to output
		mcp23008.digitalWrite(i, 1);// set pin to high (turns LED off)
	}
	for(int i=8;i<10;i++){
		mcp23017.pinMode(i, 1);// set pin to output
		mcp23017.digitalWrite(i, 1);// set pin to high (turns LED off)
	}
    
    #endif   
}

void Hackerling_Shield::setBarGraph(uint16_t bgstate){
    #if HC_VERSION == HCV1_1	
    //have two iterators going through the flags.  0-4 are on the 23008, 5-9 on the 23017
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
    #elif HC_VERSION == HCV1_2
    //have a iterator moving backwards down the leds, since the order is reversed for the mcp23008
    //LED0 is 0x0001  LED9 is 0x0200
	uint16_t b = 0x0200;
	for(int i=0;i<8;i++){
	  if(b & bgstate)
		  mcp23008.digitalWrite(i, 0);// set pin to low (turns LED on)
	  else
		  mcp23008.digitalWrite(i, 1);// set pin to high (turns LED off)
	  b = b>>1;	  
	}
	//now go forward for the first two LEDs
	b = 0x0001;
	for(int i=8;i<10;i++){
	  if(b & bgstate)
		  mcp23017.digitalWrite(i, 0);// set pin to low (turns LED on)
	  else
		  mcp23017.digitalWrite(i, 1);// set pin to high (turns LED off)
	  b = b<<1;	  
	}
    #endif   

}

//give back the Switches that are activated
uint8_t Hackerling_Shield::readSwitches(){
  //Note: the label on the dip is backwards from the variable in the schematic in version 1.1
	uint8_t svalues=0;
	//dip0 - PB0
	//dip1 - GP7
	//dip2 - GP6
	//dip3 - GP5
    #if HC_VERSION == HCV1_1	
	uint8_t readvalue = mcp23008.readGPIO();
	svalues |= (PINB & 0x01);  //dip 0
	svalues |= (readvalue >> 6) & 0x02; //dip 1
	svalues |= (readvalue >> 4) & 0x04; //dip 2
	svalues |= (readvalue >> 2) & 0x08; //dip 3
	svalues = svalues ^ 0x0f;
	svalues |= (PIND & 0x10);
    #elif HC_VERSION == HCV1_2
	uint8_t readvalue = mcp23017.readGPIOA();
	svalues=1; //physical dip 1 is not read. it controls the backlight
	svalues |= (readvalue >> 2) & 0x02; //physical dip 2   mask would be 0x08
	svalues |= (readvalue     ) & 0x04; //physical dip 3   mask would be 0x04
	svalues |= (readvalue << 2) & 0x08; //physical dip 4  mask would be 0x02
	svalues = svalues ^ 0x0f;
	svalues |= (PIND & 0x10);
	
    #endif   
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
void Hackerling_Shield::begin(uint8_t flags){
	//Dependents:
	//Serial
	  //io-expanders
    #if HC_VERSION == HCV1_1	
	mcp23017.begin(0);
	mcp23008.begin(1);
    #elif HC_VERSION == HCV1_2
	mcp23017.begin(2);
	mcp23008.begin(3);
    #endif   
	initSwitches();
	initButtons();
	initBarGraph();
	initAnalog();
	speaker.begin();
	   // Buttons
	   // Bar Graph
	   // dip switch
	if(!(flags & NO_LCD))
		lcd.begin(16,2);


	if(!(flags & NO_IR))
	IR.begin();
}



// Preinstantiate Objects //////////////////////////////////////////////////////

Hackerling_Shield hs = Hackerling_Shield();


