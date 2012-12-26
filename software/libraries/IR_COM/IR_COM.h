/*
 * IR_COM.h
 *
 *  Created on: Dec 25, 2012
 *      Author: garratt
 */

#ifndef IR_COM_H_
#define IR_COM_H_

#define IR_TX_QUEUESIZE 10


//
//signal(timer0_compare){ //or whatever this is
//	//call tx  deal:
//
//}




//This class takes care of the physical layer
//as well as the software checksum layer
class IR_Com {
	char txque[IR_TX_QUEUESIZE];
	uint8_t txpos; //position in the transmit queue
	uint8_t txnum; //number of bytes left to transmit

	uint8_t getEndPos(){
		return (txpos+txnum)%IR_TX_QUEUESIZE;
	}
	void queueByte(char b){
		uint8_t endbyte=getEndPos();
		txque[endbyte]=b;
		txnum++;
		//todo: Deal with overflow
	} //put byte in transmit queue

	uint8_t txcallnum; //counts down to indicate width of bit (phy layer)
	uint8_t bitpos;  //position in the byte that
	//called at 38Khz. returns the level at which to set the transmitter
	//return: 0 - low; 1->high; 2-> whatever we did before, 3-> off
	uint8_t txCallback(){
		if(txcallnum){
			txcallnum--;
			return 2;
		}
		//txcallnum==0, so do action
		//if in the middle of the byte:
		//set the level for the next bit
		if(txnum == 0) //nothing left to transmit
			return 3; //TODO: indicate tx complete

		bitpos=(bitpos+1)%8; //update to  next bit
		if(bitpos==0){//just flipped over
			txnum--; //update to next byte
			txpos = (txpos+1)%IR_TX_QUEUESIZE;
		}
		if(txnum == 0) //nothing left to transmit (just ran out)
			return 0; //TODO: indicate tx complete

		//otherwise, txnum>0, so set up this bit
		return txque[txpos] & 1 <<bitpos;
	}

	void flushTx(){
		bitpos=0;
		txcallnum=0;
		txpos=0;
		txnum=0;
	}

public:
	void begin() //setup LEDs and receiver
	{
	 	flushTX(); //set all phy variables
	}

	void transmitBytes(uint8_t n, char *bytes);
	void transmitRaw(char b);

};


#endif /* IR_COM_H_ */
