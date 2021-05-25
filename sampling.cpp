// This file contains functions related to reading, sampling and averaging
// the data coming from the HX711 module

// Libraries
#include <stdint.h>
#include "sampling.hpp"
#include "HX711.h"

// Captures samples from the HX711 and averages them
// Pass the HX711 object and how many samples to take as parameters
int16_t getweight(HX711 scale, uint16_t samples){
	// Reset total and avg variables for next reading
	int16_t avg = 0;
	int32_t total = 0;
	int16_t weight;

	// Capturing all samples
	for(int i = 0; i < samples; i++){
		weight = scale.get_units(samples);	// Get data from module (in kilograms)
		total += weight;										// Add reading to total
	}

	// Divides sum of all readings by the number of samples (average)
	return avg = total / samples;
}