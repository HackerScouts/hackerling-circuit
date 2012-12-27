/*
 * IR_COM.cpp
 *
 *  Created on: Dec 25, 2012
 *      Author: garratt
 */


#include <Wire.h>
#include <avr/pgmspace.h>

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif


#include "IR_COM.h"

IR_COM *global_IR_;

void IR_Receiver::addBit(){
	//shift incoming data (deal with full byte at end)
	rxbyte = rxbyte << 1; //do it this way so we can scan for a header/sync byte
	rxbyte+=rxstate; //add new bit
	rxbytepos=(rxbytepos+1)%8;
	if(rxbytepos==0){//filled up the byte
		buffer->push(rxbyte);
		//DEBUG:
		if(rxbyte==0)
			tracking=1;
	}
}
//tracking behavior:  if we start out seeing a bunch of zeros:  on the first high, start bit.
//					  if we start seeing ones, we must be in the middle of a bit, look for a 0
//							either way, we start clocking on a transition.
//tracking: 2-> have not sampled at all
//          1-> have sampled at least once, no transition so far

IR_Receiver::IR_Receiver(Circular_Buffer *b){
	buffer=b;
	init();
}
//called at 38Khz.  Sample the incoming signal.
//pass in the state of the receiver.  assumes normal encoding, and that the transmitter has the same bitlength
void IR_Receiver::rxCallback(uint8_t sig){
	if(tracking==2){
		rxstate=sig; //record current state
		tracking=1;  //mark that we have recorded a sample
		return;
	}
	if(sig == rxstate){//same direction as last time. means this bit is continuing, or new bit with same value
		if(tracking) //still waiting for a transition
			return;
		rxbitpos++; // increment bit position
//		if(rxbitpos>IR_BITLENGTH){ //Full bit received. (give allowance for +/- 1 period, and start counting at 0)
//			rxbitpos-=(IR_BITLENGTH-1);//reset bit position.
//			//We don't know exactly where the transition was, but the error will be absorbed in the transition (and is non-cumulative)
//			addBit();
//		}//rxbitpos > IR_BITLENGTH+1
	}
	else{ //if the bit transitioned
		if(tracking){ //this is the first transition
			rxbitpos=0; //position is 0th, since just had a transition
			rxbytepos=0;//have not received any bits
			rxbyte=0;   //current data state
			tracking=0; //we are no longer looking for first transition
			rxstate=sig; //record current state
			return;
		}
		lengths[transitions]=rxbitpos;
		values[transitions]=rxstate;
		transitions++;
		//if we were not tracking:
		//we could have had a bit, or an error.
		if(rxbitpos>=(IR_BITLENGTH-6)){//Full bit received. (give allowance for +/- 5 period, and start counting at 0)
			addBit(); //uses rxstate
		}
		else{//if bit was too short, may have bit error.  raise flag.
			errors++;// |= RX_BIT_ERROR;  //TODO: deal with errors (clear, etc.) somewhere
		}
		//either way, reset position and record state
		rxbitpos=0; //position is 0th, since just had a transition
		rxstate=sig; //record current state
	}
}
//initialize, start looking for a signal
void IR_Receiver::begin(){
	flush();
}
void IR_Receiver::flush(){
	tracking=2;
	errors=0;
	buffer->flush();
	transitions=0;
	for(int i=0;i<20;i++){
		lengths[i]=0;
		values[i]=0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//called at 38Khz. returns the level at which to set the transmitter
//return: 0 - low; 1->high; 2-> whatever we did before, 3-> off
uint8_t IR_Transmitter::txCallback(){
	if(txcomplete){
		if(buffer->size()==0) //TODO: investigate initializer function to eliminate this check
			return 0;
		else{//set up transmitter to pull in next byte
			txcomplete=0;
			bytepos=8;
			bitpos=IR_BITLENGTH+10;
			bitlength=1;
		}
	}
	if(bitpos<bitlength){  //still in the middle of a bit
		bitpos++;
		return 2;
	}
	bitpos=0;
	//finished bit, so do action
	//if in the middle of the byte:
	//set the level for the next bit
	bytepos=(bytepos+1)%8; //update to  next bit
	if(bytepos==0){//just finished byte, look for next
		if(buffer->size()==0){ //done with tx queue
			txcomplete=1;
			return 0;
		}
		else{ //more bytes to transmit
			txbyte=buffer->pop();
		}
	}
	bitlength=IR_TX_BITLENGTH0;
	if(txbyte & 1 <<bytepos)
		bitlength=IR_TX_BITLENGTH1;
	//set up this bit
	return txbyte & 1 <<bytepos;
}

void IR_Transmitter::flush(){
	buffer->flush();
	txcomplete=1;
	bytepos=0;
	bitpos=0;
}

void IR_Transmitter::begin() //setup LEDs and receiver
{
	flush(); //set all phy variables
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//relevant pins:
//PB4/D12 - IR_IN
//PD5/D5  - IR_Out1 (OC0B)
//PD6/D6  - IR_Out2 (OC0A)

//control the toggling of the output pins
void IR_COM::setOutput(uint8_t flag){
	if(flag==2) //do whatever we did before
		return;
	if(flag)
		TCCR0A |=0x50;
	else
		TCCR0A &= 0x0f; //turn toggling off
}

IR_COM::IR_COM():tx(&txbuffer),rx(&rxbuffer){
	global_IR_ = this;
}
void IR_COM::begin(){

	//initialize pins:
	DDRD |= 0x60;

	//start oscillator
	//TCCR0A
	// 0000xxxx - initially, do not toggle outputs on compare match
	// xxxx00xx - reserved
	// xxxxxx10 - CTC mode
	TCCR0A = 0x02;

	//TCCR0B
	// 00xxxxxx - Force output compare (don't)
	// xx00xxxx - reserved
	// xxxx0xxx - CTC mode
	// xxxxx001 - no prescaling
	TCCR0B = 0x01;
	//best freq match to 38Khz is w/ prescale=1, ctc mode, OCRnx = 210
	OCR0A = 210;
	OCR0B = 210;

	//TIMSK0 - xxxxx010 - enable Compare match A interrupt
	TIMSK0 = 0x02;


	//initialize rx, tx classes:
	rx.begin();

}

//called at 38Khz x2
void IR_COM::callback(){
	//TODO: implement locking
	//TODO: have separate tx for different LEDs
	callcount++;
	if(callcount & 0x01) return; //only call every other interrupt

	setOutput(tx.txCallback()); //the transmitter dictates if we toggle the pins or not
	uint8_t sig = (PINB & 0x10)?0:1; //flip
	rx.rxCallback(sig);
}
uint8_t IR_COM::available(){ return rxbuffer.size(); }
int IR_COM::peek(){ return rxbuffer.front(); }
int IR_COM::read(){ return rxbuffer.pop(); }
void IR_COM::txflush(){tx.flush();}
void IR_COM::rxflush(){rx.flush();}
void IR_COM::write(uint8_t d){txbuffer.push(d);}

void IR_COM::resetCount(){ //resets callcount, so it can be used as a timer
	callcount=callcount%2; //keeps parity
}

uint8_t IR_COM::countcheck(uint16_t num){
	if(num*38 > callcount)
		return 1;
	return 0;
}

void IR_COM::ConstantOn(){
	TIMSK0 = 0x00;
	TCCR0A = 0x52;
}

ISR(TIMER0_COMPA_vect)
{
    global_IR_->callback();
}
