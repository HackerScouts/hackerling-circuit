/*
 * Hackerling_Shield.h
 *
 *  Created on: Dec 23, 2012
 *      Author: garratt
 */

#ifndef HACKERLING_SHIELD_H_
#define HACKERLING_SHIELD_H_
//Here is all the pin mapping for the hackerling shield:

//MCP23017 - 16 bit io mapper:
//GPA0 - Bargraph5
#define BARGRAPH5 0
//GPA1 - Bargraph6
#define BARGRAPH6 1
//GPA2 - Bargraph7
#define BARGRAPH7 2
//GPA3 - Bargraph8
#define BARGRAPH8 3
//GPA4 - Bargraph9
#define BARGRAPH9 4
//GPA5 - LCD_E
//GPA6 - LCD_R/W
//GPA7 - LCD_RS

//GPB0 - LCD_DB0
//GPB1 - LCD_DB1
//GPB2 - LCD_DB2
//GPB3 - LCD_DB3
//GPB4 - LCD_DB4
//GPB5 - LCD_DB5
//GPB6 - LCD_DB6
//GPB7 - LCD_DB7

//MCP23008 - 8 bit io mapper:
//GPA0 - Bargraph0
//GPA1 - Bargraph1
//GPA2 - Bargraph2
//GPA3 - Bargraph3
//GPA4 - Bargraph4
//GPA5 - DIP1
//GPA6 - DIP2
//GPA7 - DIP3
#define BARGRAPH0 0
#define BARGRAPH1 1
#define BARGRAPH2 2
#define BARGRAPH3 3
#define BARGRAPH4 4


class Hackerling_Shield{
private:
	//MCP23017 class - use directly only for installation tests
	//MCP23008 class - use directly only for installation tests


public:
	//Bargraph class for controlling leds, behaviors
	//IR Comm class
	// - receiver can receive raw data, like from a tv remote
	//   transmitter can send raw data, like tv remote
	//   can be set up to send data to other boards
	//Pot class
	//Buttons class
	//dipswitch class
	//servo class
	//speaker class - depends on tone class
	//analog IO class
	//thermistor class
	//LCD class







};













#endif /* HACKERLING_SHIELD_H_ */
