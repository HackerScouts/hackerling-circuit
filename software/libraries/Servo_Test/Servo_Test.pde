//#include <Wire.h>

#include <avr/io.h>
//#include <MCP23017.h>

//#include <MCP23008.h>

//#include <Hackerling_Shield.h>

// Controlling a servo position using a potentiometer (variable resistor) 
// by Michal Rinott <http://people.interaction-ivrea.it/m.rinott> 

#include <Servo.h> 
 
Servo varservo;  // create servo object to control a servo with variable input
Servo binservo;  // create servo object to control a servo with binary input
 
int potpin = 3;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin 
 
void setup() 
{ 
//  hs.begin();
  varservo.attach(9);  // attaches the servo on pin 9 to the servo object 
  binservo.attach(10);  // attaches the servo on pin 9 to the servo object 
  
	//the big toggle switch is connected to D4:
	DDRD &= 0xef;
	PORTD |= 0x10;
} 
 
void loop() 
{ 
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023) 
  val = map(val, 0, 1023, 0, 179);     // scale it to use it with the servo (value between 0 and 180) 
  varservo.write(val);                  // sets the servo position according to the scaled value 
 // uint8_t b = hs.readButtons();
  if((PIND & 0x10))
     binservo.write(10);
else
     binservo.write(200);   
  delay(15);                           // waits for the servo to get there 
} 
