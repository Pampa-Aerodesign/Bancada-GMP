// == Controlador da bancada de teste de Torque x Corrente de servos ==

/* 
Este programa faz a medição em tempo real da corrente consumida por um servo
e exibe a leitura num display LCD 16x2.
A posição do servo pode ser controlada por um potenciômetro.
Para obter uma média de corrente com um maior número de amostras, basta
pressionar o botão HOLD. O resultado obtido será escrito no display até
que o botão seja pressionado novamente.
*/

// Libraries
#include <HX711.h>
#include <LiquidCrystal.h>
#include "sampling.hpp"
#include "display.hpp"

// Configuration
#define BAUD 57600			// Baud rate
#define DSAMPLES 1			// Number of samples to capture for calculating "real time" average
#define SAMPLES 10			// Number of samples to capture for calculating average on button press
#define DELAY	100				// Small delay to slow things down
#define DEBOUNCE 50			// Debounce delay
#define CALIB 0.0f			// Scale calibration

// Pinouts
#define HOLDPIN 6				// Digital input pin for HOLD button

// HX711 Pinout (DT, SCK)
#define HXPINS 7, 8

// LCD Pinout (RS, EN, D4, D5, D6, D7)
#define LCDPINS 12, 11, 5, 4, 3, 2


// Declaring HX711 module
HX711 scale;

//Declaring LCD display
LiquidCrystal lcd(LCDPINS);


// Global variables
bool hold = 0;					// HOLD flag (false = running; true = holding)
bool hldstate = 0;			// HOLD state
bool lasthldbtn = 0;		// Previous reading from HOLD button
uint64_t lasthldt = 0;	// Last time the HOLD flag was toggled

bool doneflag = 0;			// Flag when sensor reading is done
float weight;						// Current reading in miliamps
float avg;							// Current average
int64_t total = 0;			// Sum of all samples


void setup() {
	// Initialize serial monitor
	Serial.begin(BAUD);
	Serial.println("Initializing...");

	// Initialize display (columns, rows)
	lcd.begin(16, 2);
	lcd.print("Initializing...");

	//Set HOLD button to input
	pinMode(HOLDPIN, INPUT);

	// Initializing HX711 module
	scale.begin(HXPINS);
	scale.set_scale(CALIB);

	delay(1000);

	// Zeroing scale
	scale.tare();


	// Print debug message on display
	lcd.setCursor(0,0);				// Return cursor
	lcd.print("Complete");
	lcd.setCursor(0,1);				// Set cursor to next line
	lcd.print("Scale: ");
	lcd.print(scale.get_scale());

	// Print debug messages on serial monitor
	Serial.print("Scale set to ");
	Serial.println(scale.get_scale());
	delay(2000);

	// Clear display
	lcd.clear();

	Serial.println("Initialization complete.");
}

void loop() {
	// Variables
	int holdbtn;			// HOLD button reading

	// Reading HOLD button
	holdbtn = digitalRead(HOLDPIN);

	// Debouncing and toggling HOLD state
	if (holdbtn != lasthldbtn) {
    // reset the debouncing timer
    lasthldt = millis();
  }
	if ((millis() - lasthldt) > DEBOUNCE) {
    // whatever the reading is at, it's been there for longer than the
    // debounce delay, so take it as the actual current state

    // if the button state has changed
    if (holdbtn != hldstate) {
      hldstate = holdbtn;

			if(hldstate == HIGH){
				hold = !hold;
			}
    }
  }
	// Save button reading for the next loop
	lasthldbtn = holdbtn;

	// If HOLD is off, take readings in real time with fewer samples
	if(!hold){
		// Get reading from sensor
		avg = getweight(scale, DSAMPLES);

		// Print reading to display
		printReading(lcd, avg, hold);

		// Reset reading done flag 
		doneflag = false;
	}

	// If HOLD is on, take one reading with more samples
	else{
		if(!doneflag){
			// Get reading from sensor
			avg = getweight(scale, SAMPLES);

			// Print reading to display
			printReading(lcd, avg, hold);

			// Set reading done flag 
			doneflag = true;
		}

		delay(DELAY);
	}

	// Printing to serial monitor/plotter
	Serial.print(avg);
	Serial.print("\t");
	Serial.println(hold);

	//delay(DELAY);	// Slow down to make reading easier
}