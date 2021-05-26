// This file contains functions related
// to the LCD (16x2)

#include "display.hpp"
#include <string.h>
#include <LiquidCrystal.h>

// This function will print the value on the LCD with one decimal place
// (LCD object, float value, bool hold)
void printReading(LiquidCrystal lcd, float value, bool hold){
	char state[16];		// String indicating the state of the program

	// Select which state the program is in and copies to the string
	switch(hold){
		case true:
			strcpy(state, "HOLD           ");
			break;
		
		case false:
			strcpy(state, "Running        ");
			break;
	}

	lcd.setCursor(0,0);		// Return cursor to origin
	lcd.print(state);			// Print state
	lcd.setCursor(0,1);		// Return cursor to second line
	lcd.print("T = ");
	lcd.print(value, 1);	// Print the value with one decimal place 
	lcd.print("g");

	return;
}
