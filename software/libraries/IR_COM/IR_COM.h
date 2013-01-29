/*
 * IR_COM.h
 *
 *  Created on: Dec 25, 2012
 *      Author: garratt
 */

#ifndef IR_COM_H_
#define IR_COM_H_

#include <avr/io.h>
#define IR_TX_QUEUESIZE 10
//the length of each bit, in 38Khz periods.
//the period for 38Khz is ~26.3us, and the min period for the receiver to register is 200us
//so we'll use 10 periods, to give a good margin. based on testing, this could go as low as 8.
#define IR_BITLENGTH 37
#define IR_TX_BITLENGTH0 45 //low length
#define IR_TX_BITLENGTH1 30 //high length

#define IR_TX_HIGHLEN 40 //width of on part of signal in interrupt counts (twice per wavelength)
#define IR_TX_LOWLEN0 47 //width of off segment used to express 0 bit in interrupt counts (twice per wavelength)
#define IR_TX_LOWLEN1 124 //width of off segment used to express 1 bit in interrupt counts (twice per wavelength)
#define IR_TX_STARTLEN 600 //width of start segment (off) in interrupt counts (twice per wavelength)
#define IR_TX_POSTSTARTLEN 350 //width of on segment before off start segment in interrupt counts (twice per wavelength)

#define IR_TX_OFF   0x20    //for phy layer bookkeeping
#define IR_TX_START 0x01
#define IR_TX_0BIT  0x02
#define IR_TX_1BIT  0x04
#define IR_TX_END   0x08
#define IR_TX_POSTSTART 0x10 //for phy layer bookkeeping
#define IR_TX_HIGH 0x20 //for phy layer bookkeeping


//
//signal(timer0_compare){ //or whatever this is
//	//call tx  deal:
//
//}

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
	//copy does not erase the buffer
	uint8_t copy(uint8_t*d, uint8_t max_len){
		uint8_t _pos=pos, _dp=0;
		uint8_t _num=num;
		while(_num > 0 && max_len > _dp){
			d[_dp++] = que[_pos];
			_pos=(_pos+1)%CIRC_QUEUESIZE;
			_num--;
		}
		return _dp;
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






#define RX_BIT_ERROR 0x01
//this class handles receiver physical layer.
//it must sync to the bits coming in, as well as validate that a correct byte has been processed
//class IR_Receiver{
//public: //for debug
//	uint8_t tracking;
//	uint8_t rxbitpos; //position in bit
//	uint8_t rxbytepos; //position in byte
//	uint8_t rxstate; //whether the incoming bit is are high or low (must be 1 or 0)
//	uint8_t rxbyte; //currently incoming data
//	Circular_Buffer buffer;
//	int errors;
//
//	void addBit();  //called by the rxCallback, which gets data 1 bit at a time
////public:
//	//	debug:
//		int transitions;
//		int lengths[20];
//		int values[20];
//
//
//	//called at 38Khz.  Sample the incoming signal.
//	//pass in the state of the receiver.  assumes normal encoding, and that the transmitter has the same bitlength
//	void rxCallback(uint8_t sig);
//	//initialize, start looking for a signal
//	void begin();
//	void flush();
//};


#define IR_HIGH_LENGTH 80  // ~1000 us - division between high and low bit
#define IR_START_LENGTH 133 // ~3500 us - division between high and start bit
#define IR_IDLE_LENGTH 1000 // if the state is unchanged for a long time, ignore things

#define IR_RX_NODATA 0x01
#define IR_RX_RECEIVING 0x02
#define IR_RX_DONE 0x04
#define IR_RX_IDLE 0x10
#define IR_RX_READY 0x08

class IR_Receiver{
public:
	uint8_t last_data[30]; //the latest data stream
	uint8_t last_data_len;

	uint8_t last;
	uint8_t IRpin;
    uint8_t currentread;
	uint16_t count;
	uint8_t change_count;
	uint8_t rxbytepos; //position in byte
	uint8_t rxstate; //state of receiver: nodata,receiving,done
	uint8_t rxbyte; //currently incoming data
	Circular_Buffer buffer;

	//bitstream:
	uint8_t incoming_byte;


	void begin(int p);




	//this does not deal with start/end conditions
	void addBit();

	//if count gets > IR_START_LENGTH and we have not transitioned, we are done, or we are beginning
	//either way, if we have data in the queue, process that and get ready for new data
	//only called if count > IR_START_LENGTH
	void longBit(bool has_changed);

	void rxCallback();  //called by an external interrupt
	void rxCountCallback(); //called by the timer
};


//This class takes care of the physical layer
//as well as the software checksum layer
class IR_Transmitter {
	uint8_t bitpos; //counts up to indicate width of bit (phy layer)
	uint8_t bytepos;  //position in the byte
	uint8_t txbyte;  //byte we are currently transmitting
	uint8_t txcomplete; //done transmitting
	uint8_t bitlength; //width of current bit (high is longer than low)
public:
	Circular_Buffer buffer;
	//called at 38Khz. returns the level at which to set the transmitter
	//return: 0 - low; 1->high; 2-> whatever we did before, 3-> off
	uint8_t txCallback();
	void flush();
	void begin(); //setup LEDs and receiver

};

//relevant pins:
//PB4/D12 - IR_IN
//PD5/D5  - IR_Out1 (OC0B)
//PD6/D6  - IR_Out2 (OC0A)

//Main IR communications class.  Use this to do communication
class IR_COM{
public: //for debug
	//Circular_Buffer rxbuffer, txbuffer;
	IR_Transmitter tx;
	IR_Receiver rx;
	uint16_t callcount;
	uint8_t txstate;
	uint16_t txcount;

	//control the toggling of the output pins
	void setOutput(uint8_t flag);
//public:
	void begin();

	//called at 38Khz x2
	void callback();
	uint8_t available();
	int peek();
	uint8_t read();
    void txflush();
    void rxflush();
    void write(uint8_t d);
    void resetCount(); //resets callcount, so it can be used as a timer
    uint8_t countcheck(uint16_t num);
    //debug:
    void ConstantOn();

    inline bool isWaitingForData(){ return rx.rxstate!= IR_RX_RECEIVING;}

    inline bool hasData(){ return (rx.rxstate == IR_RX_IDLE) && rx.buffer.size() > 0;}
//	int getTrans(){ return rx.transitions;}
};


//#ifndef HACKERLING_SHIELD_H_
//extern IR_COM IR;
//#endif

extern IR_COM* _global_IR;

#endif /* IR_COM_H_ */
