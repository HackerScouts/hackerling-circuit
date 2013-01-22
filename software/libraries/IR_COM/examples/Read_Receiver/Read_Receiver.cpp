/*
 * Read_Receiver.cpp
 *
 *  Created on: Jan 20, 2013
 *      Author: garratt
 */


/*
 *  File....... IRanalyzer.pde
 *  Purpose.... Records up to 128 signal changes
 *  Author..... Walter Anderson
 *  E-mail..... wandrson@walteranderson.us
 *  Started.... 18 May 2007
 *  Updated.... 18 May 2007
 *
 *
 */
#include <avr/interrupt.h>
#include <avr/io.h>

#define CIRC_QUEUESIZE 32
class Circular_Buffer{
	char que[CIRC_QUEUESIZE];
	uint8_t pos; //position in buffer
	uint8_t num; //number of bytes in buffer (0 means empty)

	uint8_t getEndPos(){
		return (pos+num)%CIRC_QUEUESIZE;
	}
public:
	Circular_Buffer(){
		pos=0;
		num=0;
	}
	void flush(){
		pos=0;
		num=0;
	}
	void push(uint8_t d){
		uint8_t endbyte=getEndPos();
		que[endbyte]=d;
		//If overflow:
		if(num<CIRC_QUEUESIZE)
			num++;
		//TODO: Deal with overflow
	}
	int pop(){
		if(num==0)
			return -1;
		uint8_t ret = que[pos];
		pos=(pos+1)%CIRC_QUEUESIZE;
		num--;
		return ret;
	}
	int front(){
		if(num==0)
			return -1;
		return que[pos];
	}
	uint8_t size(){ return num;}
	uint8_t isFull(){ return num>=CIRC_QUEUESIZE;}
};



//#define TIMER_RESET  TCNT1 = 0
//#define SAMPLE_SIZE  68

int IRpin = 12;
//unsigned long TimerValue[SAMPLE_SIZE];
//char direction[SAMPLE_SIZE];
//byte change_count;
//long time;

#define IR_HIGH_LENGTH 38  // ~1000 us - division between high and low bit
#define IR_START_LENGTH 133 // ~3500 us - division between high and start bit

#define IR_RX_NODATA 0x01
#define IR_RX_RECEIVING 0x02
#define IR_RX_DONE 0x04




class IR_Reciever{
public:
	uint8_t last;
	uint8_t count;
	uint16_t delay_count;


	//bitstream:
	uint8_t incoming_byte;


	void begin(){
		last=0;
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
	}
	bool changed(){
		if(digitalRead(IRpin) == last)
			return false;
		last=digitalRead(IRpin);
		return true;
	}

	uint8_t rxbytepos; //position in byte
	uint8_t rxstate; //state of receiver: nodata,receiving,done
	uint8_t rxbyte; //currently incoming data
	Circular_Buffer *buffer;

	//this does not deal with start/end conditions
	void addBit(){
		//shift incoming data (deal with full byte at end)
		rxbyte = rxbyte << 1; //do it this way so we can scan for a header/sync byte
		if(count > IR_HIGH_LENGTH)
			rxbyte+=1; //add 1 bit
		rxbytepos=(rxbytepos+1)%8;
		if(rxbytepos==0){//filled up the byte
			buffer->push(rxbyte);
		}
	}

	//if count gets > IR_START_LENGTH and we have not transitioned, we are done, or we are beginning
	//either way, if we have data in the queue, process that and get ready for new data
	//only called if count > IR_START_LENGTH
	void longBit(bool has_changed){
		if(count < IR_START_LENGTH) return;
		if(has_changed && digitalRead(IRpin)>0 ){ //must actually be starting
			rxstate=IR_RX_RECEIVING; //mark that we are getting data
			rxbytepos=0;
			rxbyte=0;
			return;
		}
		//other cases:  just means we are done.
		if(rxstate != IR_RX_DONE){ //if we have not finished up
			rxstate = IR_RX_DONE;
			//do other things?
		}

	}

	void callback(){
		delay_count++;
		count++; //measuring time passed between transitions
		//figure out if direction changed, and current signal is 0
		//we measure the length of the high
		if(changed()){
			if(digitalRead(IRpin)==0){//get the time that the pin has been high
				if(count > IR_START_LENGTH)
					longBit(true); //starts or stops sequence
				else
					addBit();  //records bit
			}
			count=0;//either way, mark time we went transitioned...
		}
		else{
			if(count > IR_START_LENGTH+30){
				longBit(false); //allow detection of no data
				count-=29; //this sets the frequency that we will 'recheck' done state
				//and also makes sure count does not wrap around
			}
		}
	}

};

IR_Reciever global_IR_;

//delays ms milliseconds
void gdelay(uint16_t ms){
	uint16_t _end=ms*38;
	global_IR_.delay_count=0;
	while (global_IR_.delay_count<_end);
}

ISR(TIMER0_COMPA_vect)
{
    global_IR_.callback();
}




void setup() {
  Serial.begin(9600);
  global_IR_.begin();
  Serial.println("Analyze IR Remote");
//  TCCR1A = 0x00;          // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,PWM10=0 => PWM Operation disabled
//  // ICNC1=0 => Capture Noise Canceler disabled -- ICES1=0 => Input Capture Edge Select (not used) -- CTC1=0 => Clear Timer/Counter 1 on Compare/Match
//  // CS12=0 CS11=1 CS10=1 => Set prescaler to clock/64
//  TCCR1B = 0x03;          // 16MHz clock with prescaler means TCNT1 increments every 4uS
//  // ICIE1=0 => Timer/Counter 1, Input Capture Interrupt Enable -- OCIE1A=0 => Output Compare A Match Interrupt Enable -- OCIE1B=0 => Output Compare B Match Interrupt Enable
//  // TOIE1=0 => Timer 1 Overflow Interrupt Enable
//  TIMSK1 = 0x00;
  pinMode(IRpin, INPUT);
}

void loop()
{
  Serial.println("Waiting...");
  while(global_IR_.rxstate!= IR_RX_RECEIVING)
	  gdelay(1);
  Serial.println("Bit stream detected!");
  //wait for done state...
  while(global_IR_.rxstate!= IR_RX_DONE)
	  gdelay(10);
  Serial.println("Bit stream end!");
  while(global_IR_.buffer->size() > 0)
	  Serial.print(global_IR_.buffer->pop(),HEX);
  Serial.print("\n");
  gdelay(2000);
}

