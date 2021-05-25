// This file contains functions related to reading, sampling and averaging
// the data coming from the HX711 module

// Libraries
#include <stdint.h>
#include "sampling.hpp"
#include "HX711.h"

// Captures samples from the HX711 and averages them
// Pass the HX711 object and how many samples to take as parameters
float getweight(HX711 scale, uint16_t samples){
	float weight;

	// Capturing all samples
	weight = scale.get_units(samples);	// Get data from module (in grams)

	// Return reading
	return weight;
}