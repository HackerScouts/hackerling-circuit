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

void setup() {
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows:
	hs.begin();
	Serial.print("Step 13 Verification:\n Test passes if you hear a tone.\n");


}

void marylamb(){
	uint16_t scale=3;
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_C,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_E,40*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_D,40*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_G,20*scale);
	hs.speaker.playNote(NOTE_G,40*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_C,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_E,40*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_E,20*scale);
	hs.speaker.playNote(NOTE_D,20*scale);
	hs.speaker.playNote(NOTE_C,60*scale);
}

void starwars(){
	uint16_t scale=3;
        hs.speaker.setOctave(2);
	hs.speaker.playNote(NOTE_G,40*scale);
	hs.speaker.playNote(NOTE_G,40*scale);
	hs.speaker.playNote(NOTE_B,30*scale);
	hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_G,30*scale);
	hs.speaker.playNote(NOTE_D,30*scale);
        hs.speaker.playNote(NOTE_C,10*scale);
        hs.speaker.setOctave(2);
	hs.speaker.playNote(NOTE_B,10*scale);
	hs.speaker.playNote(NOTE_A,10*scale);

	hs.speaker.setOctave(4);
	hs.speaker.playNote(NOTE_G,30*scale);
	hs.speaker.playNote(NOTE_D,30*scale);
        hs.speaker.playNote(NOTE_C,10*scale);
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_B,10*scale);
	hs.speaker.playNote(NOTE_A,10*scale);


	hs.speaker.setOctave(4);
	hs.speaker.playNote(NOTE_G,30*scale);
	hs.speaker.playNote(NOTE_D,30*scale);
        hs.speaker.playNote(NOTE_C,10*scale);
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_B,10*scale);
	hs.speaker.setOctave(4);
	hs.speaker.playNote(NOTE_C,10*scale);
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_A,50*scale);

}
void starwars2(){
	uint16_t scale=3;
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_Cs,10*scale);
	hs.speaker.playNote(NOTE_D,10*scale);
	hs.speaker.playNote(NOTE_D,10*scale);
        hs.speaker.setOctave(2);
        hs.speaker.playNote(NOTE_A,10*scale);
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_Gs,30*scale);
	hs.speaker.setOctave(4);
	hs.speaker.playNote(NOTE_B,30*scale);
        hs.speaker.setOctave(3);
        hs.speaker.playNote(NOTE_A,10*scale);
	hs.speaker.playNote(NOTE_Cs,10*scale);
	hs.speaker.playNote(NOTE_D,10*scale);
        hs.speaker.setOctave(2);
        hs.speaker.playNote(NOTE_A,10*scale);
        hs.speaker.setOctave(3);
	hs.speaker.playNote(NOTE_Gs,30*scale);
	hs.speaker.setOctave(4);
	hs.speaker.playNote(NOTE_B,30*scale);
        hs.speaker.setOctave(3);
        hs.speaker.playNote(NOTE_A,10*scale);
	hs.speaker.playNote(NOTE_Cs,10*scale);
	hs.speaker.playNote(NOTE_D,10*scale);



}
//G D, CBA >GD CBA >GD CBC A... (repeat)
//C FEDCCDED ABG, C FEDCCDED GD
 
//C G FED C G FED CG FEFD, C G FED C G FED CG FEFD




void scales(){
	for(int i=1;i<5;i++){
		hs.speaker.setOctave(i);
		hs.speaker.playNote(NOTE_C,300);
		hs.speaker.playNote(NOTE_D,300);
		hs.speaker.playNote(NOTE_E,300);
		hs.speaker.playNote(NOTE_F,300);
		hs.speaker.playNote(NOTE_G,300);
		hs.speaker.playNote(NOTE_A,300);
		if(i<4){//B is on the other side...
		hs.speaker.setOctave(i+1);
		hs.speaker.playNote(NOTE_B,300);
		}
		else
			hs.speaker.playNote(NOTE_Balt,300);
	}
}


uint8_t playcount=0;
uint8_t state=0;
uint8_t prev=3;

//stop this from playing so much...
bool annoying(){
	if(playcount>5){
		delay(1000);
		return true;
	}
	state=hs.readSwitches();
	if((state & 0x10) == prev){
		delay(100);
		return true;
	}
	prev = (state & 0x10);
	playcount++;
	return false;
}



void loop() {
	if(annoying())
		return;
	marylamb();
	delay(500);
}
