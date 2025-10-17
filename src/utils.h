#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/**
 * Calculate the median value from an array of uint16_t values
 * @param values Array of uint16_t values
 * @param count Number of values in the array
 * @return Median value
 * @note This function modifies the input array by sorting it
 */
uint16_t calculateMedian(uint16_t* values, uint16_t count);

/**
 * Calculate the median value from an array of float values
 * @param values Array of float values
 * @param count Number of values in the array
 * @return Median value
 * @note This function modifies the input array by sorting it
 */
float calculateMedian(float* values, uint16_t count);

#endif // UTILS_H