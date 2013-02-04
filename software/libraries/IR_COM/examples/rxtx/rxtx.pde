#include <IR_COM.h>
#include <Hackerling_Shield.h>
#include <MCP23008.h>
#include <Wire.h>
#include <MCP23017.h>
#include <LCD.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>

uint8_t savedIR[10];
uint8_t savedIR_len;

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

void togglefake(){
  if(TIMSK0 & 0x01)
    TIMSK0 &= 0xfe;
  else
    TIMSK0 |= 0x01;
}



// TInitialize the LCD class
LCD lcd = LCD();



void setup() {
  Serial.begin(19200);
  Serial.println("Analyze IR Remote");

  DDRB |=0x06; //enable LEDs
  PORTB &= 0xf9;
  
  
  hs.begin();
  Serial.println("Hackerling Shield");
  lcd.begin(16, 2);
  
  lcd.print("LCD Ready!  ");
  Serial.println("LCD");
  
  IR.begin();
  Serial.println("IR UP");
  
savedIR_len=0;
//  DDRB |=0x06; //enable LEDs
  
  
//toggleLED2();
 // lcd.print("Full");
//toggleLED1();
}

uint8_t buttons;
uint8_t lcount=0;
void loop()
{
  Serial.println("Waiting...");
 // lcd.setCursor(0, 1); lcd.print("Waiting...");
  while(IR.rx.rxstate!= IR_RX_RECEIVING){
    buttons= hs.readButtons();
   if(buttons & BUTTON_DOWN){
      lcd.setCursor(0, 1);
      lcd.print("Trans: ");
         for(int i=0;i<savedIR_len; i++){
            IR.write(savedIR[i]);
            lcd.print(savedIR[i]>>4,HEX);
            lcd.print(savedIR[i]&0xf,HEX);
         }
         hs.speaker.playNote(NOTE_C,60000);
         hs.speaker.playNote(NOTE_F,60000);
   }
 //   delayMicroseconds(10);
  }
  Serial.println("Bit stream detected!");

  //wait for done state...
  while(IR.rx.rxstate != IR_RX_IDLE)  {togglefake(); _delay_loop_2(500);}
  Serial.println("Bit stream end!");
  uint16_t cumcount=0;
  #if 0
  for(int i=0;i<change_count; i++){
    cumcount+=TimerValue[i];
    Serial.print((int)cumcount);Serial.print(" ");
    Serial.print((int)TimerValue[i]);Serial.print(" ");
    Serial.print((int)(direction[i] & 0x01));Serial.print(" ");
    Serial.print((int)((direction[i] >>4) & 0x01));Serial.print(" ");
    Serial.print((int)((direction[i] >>5) & 0x01));Serial.print(" ");
    Serial.println((int)(direction[i] >>6));
  }
  #endif
  
uint8_t buff;
lcd.setCursor(0, 1);
      lcd.print("                ");
lcd.setCursor(0, 1);
savedIR_len=0;
  while(IR.rx.buffer.size() > 0){
    buff=IR.rx.buffer.pop();
	  Serial.print(buff,HEX);
          lcd.print(buff>>4,HEX);
          lcd.print(buff&0xf,HEX);
          if(savedIR_len<9) //no overflows...
            savedIR[savedIR_len++]=buff;
  }
  
         hs.speaker.playNote(NOTE_E,60000);
         hs.speaker.playNote(NOTE_B,60000);
         hs.speaker.playNote(NOTE_E,60000);
  Serial.print("\n");
}


