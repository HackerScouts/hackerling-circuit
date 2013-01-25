/*
 * IR_COM.cpp
 *
 *  Created on: Dec 25, 2012
 *      Author: garratt
 */


#include <Wire.h>
#include <avr/pgmspace.h>
#include "IR_COM.h"

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

void toggleLED1(){
  if(PORTB & 0x04)
    PORTB &= 0xfb;
  else
    PORTB |= 0x04;
}

void toggleLED2(){
  if(PORTB & 0x02)
    PORTB &= 0xfd;
  else
    PORTB |= 0x02;
}

SIGNAL(TIMER0_COMPA_vect)
{
    IR.callback();
}


void IR_Receiver::addBit(){
	//shift incoming data (deal with full byte at end)
	rxbyte = rxbyte << 1; //do it this way so we can scan for a header/sync byte
	rxbyte+=rxstate; //add new bit
	rxbytepos=(rxbytepos+1)%8;
	if(rxbytepos==0){//filled up the byte
		buffer.push(rxbyte);
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

//IR_Receiver::IR_Receiver(Circular_Buffer *b){
//	buffer=b;
//	init();
//}
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
	buffer.flush();
	transitions=0;
	for(int i=0;i<20;i++){
		lengths[i]=0;
		values[i]=0;
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//in IR communication (or atleast that used by most remotes) 1/0 is determined by the length of off segments
//of course, the whole system is modulated at 38KHz
//#define IR_TX_HIGHLEN 40 //width of on part of signal in interrupt counts (twice per wavelength)
//#define IR_TX_LOWLEN0 47 //width of off segment used to express 0 bit in interrupt counts (twice per wavelength)
//#define IR_TX_LOWLEN1 124 //width of off segment used to express 1 bit in interrupt counts (twice per wavelength)
//#define IR_TX_STARTLEN 255 //width of start segment (off) in interrupt counts (twice per wavelength)
//#define IR_TX_POSTSTARTLEN 200 //width of on segment before off start segment in interrupt counts (twice per wavelength)
//
//#define IR_TX_OFF   0x00    //for phy layer bookkeeping
//#define IR_TX_START 0x01
//#define IR_TX_0BIT  0x02
//#define IR_TX_1BIT  0x04
//#define IR_TX_END   0x08
//#define IR_TX_POSTSTART 0x10 //for phy layer bookkeeping
//#define IR_TX_HIGH 0x20 //for phy layer bookkeeping
//called at 76Khz. returns the level at which to set the transmitter
//tx callback gives back a few different responses:
//1) Start segment - turn on transmitter for IR_TX_STARTLEN, then turn off for IR_TX_LOWLEN
//2) 0 segment - turn on transmitter for IR_TX_HIGHLEN0, then turn off for IR_TX_LOWLEN
//3) 1 segment - turn on transmitter for IR_TX_HIGHLEN0, then turn off for IR_TX_LOWLEN
//4) end segment - turn off transmitter and check back every IR_TX_STARTLEN

uint8_t IR_Transmitter::txCallback(){
	if(txcomplete){ //in non-transmitting mode
	//	PORTB &= 0xfb; //DEBUG
		if(buffer.size()==0) //done transmitting, do nothing
			return IR_TX_OFF;
		else{//if just starting. deal with setting up bit next iteration. now just do start bit
			txcomplete=0;
			bytepos=0;
			return IR_TX_START;
		}
	}
	 //   PORTB |= 0x04; //DEBUG

	//in transmitting mode
	if(bytepos)//in the middle of the byte
		bytepos--;
	else{//bytepos==0, so we either have to end, or pull in the next byte
		if(buffer.size()){//more to transmit
			txbyte=buffer.pop();
			bytepos=7;
		}
		else{//nothing left to transmit. shut it down.
			txcomplete=1;
			toggleLED1();
			return IR_TX_END;
		}
	}
	//now, if we haven't returned, we are sending the bit at bytepos.
	if(txbyte & (1 <<bytepos))
		return IR_TX_1BIT;
	return IR_TX_0BIT;
//
//
//	if(txcomplete){
//		if(buffer.size()==0) //TODO: investigate initializer function to eliminate this check
//			return 0;
//		else{//set up transmitter to pull in next byte
//			txcomplete=0;
//			bytepos=8;
//			bitpos=IR_BITLENGTH+10;
//			bitlength=1;
//		}
//	}
//	if(bitpos<bitlength){  //still in the middle of a bit
//		bitpos++;
//		return 2;
//	}
//	bitpos=0;
//	//finished bit, so do action
//	//if in the middle of the byte:
//	//set the level for the next bit
//	bytepos=(bytepos+1)%8; //update to  next bit
//	if(bytepos==0){//just finished byte, look for next
//		if(buffer.size()==0){ //done with tx queue
//			txcomplete=1;
//			return 0;
//		}
//		else{ //more bytes to transmit
//			txbyte=buffer.pop();
//		}
//	}
//	bitlength=IR_TX_BITLENGTH0;
//	if(txbyte & 1 <<bytepos)
//		bitlength=IR_TX_BITLENGTH1;
//	//set up this bit
//	return txbyte & 1 <<bytepos;
}

void IR_Transmitter::flush(){
	buffer.flush();
	txcomplete=1;
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

void IR_COM::begin(){
	txstate=IR_TX_OFF;
	txcount=0;
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
	//first deal with receiver
//	uint8_t sig = (PINB & 0x10)?0:1; //flip
//	rx.rxCallback(sig);
	//txstate indicates what part of transmitting we are doing - high bit or low bit
	//after every low bit comes a high bit
	//txcount tells us the interrupts left until we change

//	if(tx.buffer.size())
//	    PORTB |= 0x04;
//	  else
//		PORTB &= 0xfb;

	callcount++; //for keeping track of time for delays
	if(txcount)
		txcount--;
	else{//if txcount=0, must make transition.
		//if(callcount%1000==0)
		//	toggleLED2();


		if(txstate & (IR_TX_POSTSTART|IR_TX_0BIT|IR_TX_1BIT)){ //if we just did the low part, transmit a high pulse
			txstate = IR_TX_HIGH;
			TCCR0A |=0x50; //turn toggling on
			txcount=IR_TX_HIGHLEN;
			return;
		}
		if(txstate & (IR_TX_HIGH | IR_TX_OFF)){//check for next bit
			txstate = tx.txCallback(); //now set to start, 0bit, 1bit, end, off
			if(txstate == IR_TX_0BIT){
				txcount=IR_TX_LOWLEN0;
				TCCR0A &= 0x0f; //turn toggling off
				return;
			}
			if(txstate == IR_TX_1BIT){
				txcount=IR_TX_LOWLEN1;
				TCCR0A &= 0x0f; //turn toggling off
				return;
			}
			if(txstate & (IR_TX_START)){
				txcount=IR_TX_STARTLEN; //and keep toggling on
				TCCR0A |=0x50; //turn toggling on
				return;
			}
			if(txstate == IR_TX_OFF|IR_TX_END){
				txcount=255;//max
				TCCR0A &= 0x0f; //turn toggling off
				return;
			}
			return; //should never get here, unless txstate is tainted
		}
		if(txstate & (IR_TX_START | IR_TX_END)){
			TCCR0A &= 0x0f; //turn toggling off
			txcount=IR_TX_POSTSTARTLEN;
			if(txstate == (IR_TX_START))
				txstate = IR_TX_POSTSTART;
			else
				txstate = IR_TX_OFF;
			return;
		}
	}//end if txcount==0

}
uint8_t IR_COM::available(){ return rx.buffer.size(); }
int IR_COM::peek(){ return rx.buffer.front(); }
int IR_COM::read(){ return rx.buffer.pop(); }
void IR_COM::txflush(){tx.flush();}
void IR_COM::rxflush(){rx.flush();}
void IR_COM::write(uint8_t d){tx.buffer.push(d);}

void IR_COM::resetCount(){ //resets callcount, so it can be used as a timer
	callcount=0; //keeps parity
}

uint8_t IR_COM::countcheck(uint16_t num){
	if(num*38 > callcount)
		return 1;
	return 0;
}

void IR_COM::ConstantOn(){
//	TIMSK0 = 0x00;
//	TCCR0A = 0x52;
}

