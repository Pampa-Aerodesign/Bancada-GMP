// This file contains the function to set tare weight (zero the scale)

#include "tare.hpp"
#include <HX711.h>
#include <LiquidCrystal.h>

void set_tare(HX711 scale, LiquidCrystal lcd){
	// Print reading message
	lcd.setCursor(0,0);
	lcd.print("Reading...");
	Serial.print("Reading...");

	// Zeroing the scale
	scale.tare();

	// Print tare message
	lcd.setCursor(0,0);
	lcd.print("Tare set");
	Serial.println("Tare set");

	return;
}
	