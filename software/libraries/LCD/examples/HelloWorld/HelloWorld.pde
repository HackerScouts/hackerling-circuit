#include <IR_COM.h>

/*********************

Example code for the Adafruit RGB Character LCD Shield and Library

This code displays text on the shield, and also reads the buttons on the keypad.
When a button is pressed, the backlight changes color.

**********************/

// include the library code:
#include <Wire.h>
#include <MCP23017.h>
#include <Hackerling_Shield.h>
#include <MCP23008.h>
#include <LCD.h>

// TInitialize the LCD class
LCD lcd = LCD();
uint8_t userstr[12];
uint8_t pos=0;


void setup() {
  // Debugging output
  Serial.begin(9600);
  // set up the LCD's number of columns and rows: 
  hs.begin();
  lcd.begin(16, 2);

  // Print a message to the LCD. We track how long it takes
  int time = millis();
  lcd.print("Hello, world!");
  time = millis() - time;
  Serial.print("Took "); Serial.print(time); Serial.println(" ms");
  for(int i=0;i<12;i++)
	  userstr[i]=32;
}

uint8_t i=0;
void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  for(int i=0;i<12;i++)
  	  lcd.print((char)userstr[i]);
  lcd.setCursor(pos, 1);
  lcd.cursor();

  userstr[pos]= hs.getPotValue()/2;




  uint8_t buttons = hs.readButtons();
  hs.setBarGraph(buttons);
  if (buttons){
	  if(buttons & BUTTON_LEFT){
		  if(pos>0)
			  pos--;
	  }
	  if(buttons & BUTTON_RIGHT){
		  if(pos<11)
			  pos++;
	  }
  }
  lcd.noCursor();
  lcd.setCursor(13, 0);
  lcd.print((int)hs.getThermistorValue());
//  lcd.print(millis()/1000);
  lcd.print(" ");
  lcd.setCursor(13, 1);
  lcd.print((int)hs.getPotValue()/2);
  lcd.print(" ");
  lcd.setCursor(pos, 1);
  lcd.print((char)255);
  lcd.setCursor(pos, 1);
//  lcd.print((char)userstr[pos]);

}
