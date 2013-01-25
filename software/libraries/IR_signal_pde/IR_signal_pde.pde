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

#define TIMER_RESET  TCNT1 = 0
#define SAMPLE_SIZE  68

int IRpin = 12;
unsigned long TimerValue[SAMPLE_SIZE];
char direction[SAMPLE_SIZE];
byte change_count;
long time;

void setup() {
  Serial.begin(9600);
  Serial.println("Analyze IR Remote");
  TCCR1A = 0x00;          // COM1A1=0, COM1A0=0 => Disconnect Pin OC1 from Timer/Counter 1 -- PWM11=0,PWM10=0 => PWM Operation disabled
  // ICNC1=0 => Capture Noise Canceler disabled -- ICES1=0 => Input Capture Edge Select (not used) -- CTC1=0 => Clear Timer/Counter 1 on Compare/Match
  // CS12=0 CS11=1 CS10=1 => Set prescaler to clock/64
  TCCR1B = 0x03;          // 16MHz clock with prescaler means TCNT1 increments every 4uS
  // ICIE1=0 => Timer/Counter 1, Input Capture Interrupt Enable -- OCIE1A=0 => Output Compare A Match Interrupt Enable -- OCIE1B=0 => Output Compare B Match Interrupt Enable
  // TOIE1=0 => Timer 1 Overflow Interrupt Enable
  TIMSK1 = 0x00;          
  pinMode(IRpin, INPUT);
}

void loop()
{
  Serial.println("Waiting...");
  change_count = 0;
  while(digitalRead(IRpin) == HIGH) {}                                 
  TIMER_RESET;
  TimerValue[change_count] = TCNT1;
  direction[change_count++] = '0';
  while (change_count < SAMPLE_SIZE) {
    if (direction[change_count-1] == '0') {
      while(digitalRead(IRpin) == LOW) {}
      TimerValue[change_count] = TCNT1;
      direction[change_count++] = '1';
    } else {
      while(digitalRead(IRpin) == HIGH) {}
      TimerValue[change_count] = TCNT1;
      direction[change_count++] = '0';
    }
  }
  Serial.println("Bit stream detected!");
  change_count = 0;
  time = (long) TimerValue[change_count] * 4;
  Serial.print(time);
  Serial.print("\t");
  Serial.println(direction[change_count++]);
  while (change_count < SAMPLE_SIZE) {
    time = (long) TimerValue[change_count] * 4;
    Serial.print(time);
    Serial.print("\t");
    Serial.println(direction[change_count-1]);
    Serial.print(time);
    Serial.print("\t");
    Serial.println(direction[change_count++]);    
  }
  Serial.println("Bit stream end!");
  delay(2000);
}
