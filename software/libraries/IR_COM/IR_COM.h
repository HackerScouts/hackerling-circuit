/*
 * IR_COM.h
 *
 *  Created on: Dec 25, 2012
 *      Author: garratt
 */

#ifndef IR_COM_H_
#define IR_COM_H_

#define IR_TX_QUEUESIZE 10
//the length of each bit, in 38Khz periods.
//the period for 38Khz is ~26.3us, and the min period for the receiver to register is 200us
//so we'll use 10 periods, to give a good margin. based on testing, this could go as low as 8.
#define IR_BITLENGTH 37
#define IR_TX_BITLENGTH0 45 //low length
#define IR_TX_BITLENGTH1 30 //high length
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
class IR_Receiver{
public: //for debug
	uint8_t tracking;
	uint8_t rxbitpos; //position in bit
	uint8_t rxbytepos; //position in byte
	uint8_t rxstate; //whether the incoming bit is are high or low (must be 1 or 0)
	uint8_t rxbyte; //currently incoming data
	Circular_Buffer *buffer;
	int errors;

	void addBit();  //called by the rxCallback, which gets data 1 bit at a time
//public:
	//	debug:
		int transitions;
		int lengths[20];
		int values[20];


	IR_Receiver(Circular_Buffer *b);
	//called at 38Khz.  Sample the incoming signal.
	//pass in the state of the receiver.  assumes normal encoding, and that the transmitter has the same bitlength
	void rxCallback(uint8_t sig);
	//initialize, start looking for a signal
	void begin();
	void flush();
};

//This class takes care of the physical layer
//as well as the software checksum layer
class IR_Transmitter {
	Circular_Buffer *buffer;
	uint8_t bitpos; //counts up to indicate width of bit (phy layer)
	uint8_t bytepos;  //position in the byte
	uint8_t txbyte;  //byte we are currently transmitting
	uint8_t txcomplete; //done transmitting
	uint8_t bitlength; //width of current bit (high is longer than low)
public:
	//called at 38Khz. returns the level at which to set the transmitter
	//return: 0 - low; 1->high; 2-> whatever we did before, 3-> off
	uint8_t txCallback();
	void flush();
	IR_Transmitter(Circular_Buffer *b){ buffer=b;}
	void begin(); //setup LEDs and receiver

};

//relevant pins:
//PB4/D12 - IR_IN
//PD5/D5  - IR_Out1 (OC0B)
//PD6/D6  - IR_Out2 (OC0A)

//Main IR communications class.  Use this to do communication
class IR_COM{
public: //for debug
	Circular_Buffer rxbuffer, txbuffer;
	IR_Transmitter tx;
	IR_Receiver rx;
	uint16_t callcount;

	//control the toggling of the output pins
	void setOutput(uint8_t flag);
//public:
	IR_COM();
	void begin();

	//called at 38Khz x2
	void callback();
	uint8_t available();
	int peek();
	int read();
    void txflush();
    void rxflush();
    void write(uint8_t d);
    void resetCount(); //resets callcount, so it can be used as a timer
    uint8_t countcheck(uint16_t num);
    //debug:
    void ConstantOn();
	int getTrans(){ return rx.transitions;}
};



#endif /* IR_COM_H_ */
