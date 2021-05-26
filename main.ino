// == Controlador da bancada do grupo motopropulsor ==

/* 
Este programa faz a medição em tempo real de uma célula de carga
e exibe a leitura num display LCD 16x2 e no monitor serial.
Para ter uma leitura com maior número de amostras, basta pressionar
uma vez o botão HOLD. A média das leituras será exibida no display.
Para zerar a leitura da célula de carga (tara), basta segurar
o botão HOLD por dois segundos.

O número de amostragens em tempo real e em hold pode ser
ajustado nos defines abaixo.
*/

// Libraries
#include <HX711.h>
#include <LiquidCrystal.h>
#include "sampling.hpp"
#include "display.hpp"
#include "tare.hpp"

// Configuration
#define BAUD 57600			// Baud rate
#define DSAMPLES 2			// Number of samples to capture for calculating "real time" average
#define SAMPLES 20			// Number of samples to capture for calculating average on button press
#define HLDDELAY 50			// HOLD debounce delay
#define TAREDELAY 2000	// Tare button delay
#define DELAY	100				// Generic delay
#define CALIB 0.0f			// Scale calibration

// Pinouts
#define HOLDPIN 13				// Digital input pin for HOLD button

// HX711 Pinout (DT, SCK)
#define HXPINS 11, 12

// LCD Pinout (RS, EN, D4, D5, D6, D7)
#define LCDPINS 2, 3, 4, 5, 6, 7


// Declaring HX711 module
HX711 scale;

//Declaring LCD display
LiquidCrystal lcd(LCDPINS);


// Global variables
bool hold = 0;					// HOLD flag (false = running; true = holding)
bool lasthldbtn = 1;		// Previous reading from HOLD button
uint64_t lasthldt = 0;	// Last time the HOLD flag was toggled
unsigned long btntime;		// How long the button has been pressed

bool doneflag = 0;			// Flag when sensor reading is done
float weight;						// Weight reading in grams
int64_t total = 0;			// Sum of all samples


void setup() {
	// Initialize serial monitor
	Serial.begin(BAUD);
	Serial.println("Initializing...");

	// Initialize display (columns, rows)
	lcd.begin(16, 2);
	lcd.print("Initializing...");

	//Set HOLD button to input
	pinMode(HOLDPIN, INPUT_PULLUP);

	// Initializing HX711 module
	scale.begin(HXPINS);
	scale.set_scale(CALIB);

	delay(1000);

	// Print debug message on display
	lcd.setCursor(0,0);				// Return cursor
	lcd.print("Complete       ");
	lcd.setCursor(0,1);				// Set cursor to next line
	lcd.print("Scale: ");
	lcd.print(scale.get_scale());

	// Print debug messages on serial monitor
	Serial.print("Scale set to ");
	Serial.println(scale.get_scale());
	delay(2000);

	// Zeroing the scale
	set_tare(scale, lcd);

	// Clear display
	lcd.clear();

	Serial.println("Initialization complete.");
}

void loop() {
	// Variables
	int holdbtn;								// HOLD button reading

	// Reading HOLD button
	holdbtn = digitalRead(HOLDPIN);

	// Debouncing and toggling HOLD state
	// Store the time when the button was pressed
	if(holdbtn != lasthldbtn && !holdbtn){
    lasthldt = millis();						// Time when button was pressed
  }

	// If the button is still being pressed, calculate for how long
	if(!holdbtn){
		btntime = millis() - lasthldt;	// How long the button has been pressed
	}

	// If button is pressed for more than debounce delay, 
	// but released in less than 2 seconds, toggle HOLD state
	if(btntime > HLDDELAY && btntime < TAREDELAY && holdbtn){
		hold = !hold;
		btntime = 0;
  }

	// If button is held for more than 2 seconds, set tare weight
	else if(btntime > TAREDELAY){

		// Zeroing the scale
		set_tare(scale, lcd);

		delay(2000);
		btntime = 0;
	}
	// Save button reading for the next loop
	lasthldbtn = holdbtn;
	

	// If HOLD is off, take readings in real time with fewer samples
	if(!hold){
		// Get reading from sensor
		weight = getweight(scale, DSAMPLES);

		// Print reading to display
		printReading(lcd, weight, hold);

		// Reset reading done flag 
		doneflag = false;
	}

	// If HOLD is on, take one reading with more samples and keep it on screen
	else{
		if(!doneflag){
			// Print tare message on display
			lcd.setCursor(0,0);
			lcd.print("Reading...     ");
			Serial.println("Reading...");

			// Get reading from sensor
			weight = getweight(scale, SAMPLES);

			// Print reading to display
			printReading(lcd, weight, hold);

			// Set reading done flag 
			doneflag = true;
		}

		delay(DELAY);
	}

	// Printing to serial monitor/plotter
	Serial.print(weight);		// Print weight
	Serial.print("\t");
	Serial.print(btntime);	// somehow if you remove these two lines
	Serial.print("\t");			// the program stops working
	Serial.println(hold);		// Print "HOLD" or "Running" message
}