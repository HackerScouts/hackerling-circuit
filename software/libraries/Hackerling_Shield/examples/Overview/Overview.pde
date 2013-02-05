#include <Hackerling_Shield.h>
#include <IR_COM.h>
#include <MCP23008.h>

#include <Wire.h>
#include <MCP23017.h>

#include <LCD.h>

//----------------------GLOBAL VARIABLES---------------------------------------

uint8_t savedIR[32];                        //Saves IR data
uint8_t savedIR_len;                        //Tells us how many characters of IR data we have

//We also have the following variable that was declared in the header file: (this is rare)
//Hackerling_Shield hs;



void printRecievedData(){                    //This is a globally defined program.
  uint8_t buff;
  hs.lcd.setCursor(0, 1);
  hs.lcd.print("                ");
  hs.lcd.setCursor(0, 1);
  savedIR_len=0;
  while(hs.IR.rx.buffer.size() > 0){
    buff=hs.IR.rx.buffer.pop();
	  Serial.print(buff,HEX);
          hs.lcd.print(buff>>4,HEX);
          hs.lcd.print(buff&0xf,HEX);
  }
}

void trasmitSavedData(){
      hs.lcd.setCursor(0, 1);
      hs.lcd.print("Trans: ");
      for(int i=0;i<savedIR_len; i++){
         hs.IR.write(savedIR[i]);
         hs.lcd.print(savedIR[i]>>4,HEX);
         hs.lcd.print(savedIR[i]&0xf,HEX);
       }
}




void setup()                                 //The Setup Function. This gets run once.
{
  Serial.begin(19200);                        //Initialize the Serial port, so we can communicate to the computer
  Serial.println("Analyze IR Remote");        //We can now write things to the computer like this

  hs.begin();                                 //Initialize the Hackerling Shield, so we can use all the stuff on it
  hs.lcd.print("LCD Ready!  ");               //We can print on the LCD using this command

  savedIR_len=0;                              //we have no saved IR codes yet
  Serial.println("Waiting...");

}


void loop()                                 //The Loop Function. This gets run forever
{
   uint8_t buttons= hs.readButtons();          //read the buttons - see which ones are pressed
   if(buttons == BUTTON_DOWN){
         trasmitSavedData();
         hs.speaker.playNote(NOTE_C,200);
         hs.speaker.playNote(NOTE_F,200);
   }


  //wait for done state...
  if( hs.IR.hasData() ){
       //Print the data on the LCD and over the USB:
       printRecievedData();                                        //calls the function at the top of this program
       
       //Play a little sound to indicate we got data:
       hs.speaker.playNote(NOTE_E,600);
       hs.speaker.playNote(NOTE_B,400);
       hs.speaker.playNote(NOTE_E,400);
       
       //Save the data so we can send it out:
       for(savedIR_len=0; savedIR_len<hs.IR.rx.last_data_len && savedIR_len < 32; savedIR_len++){
         savedIR[savedIR_len] =  hs.IR.rx.last_data[savedIR_len];
       }
  }
}

