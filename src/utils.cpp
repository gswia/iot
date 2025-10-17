#include "utils.h"

/**
 * Simple bubble sort for uint16_t array
 * @param values Array to sort
 * @param count Number of elements
 */
static void bubbleSort(uint16_t* values, uint16_t count) {
    for (uint16_t i = 0; i < count - 1; i++) {
        for (uint16_t j = 0; j < count - i - 1; j++) {
            if (values[j] > values[j + 1]) {
                // Swap elements
                uint16_t temp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = temp;
            }
        }
    }
}

uint16_t calculateMedian(uint16_t* values, uint16_t count) {
    // Handle edge cases
    if (values == nullptr || count == 0) {
        return 0;
    }
    
    if (count == 1) {
        return values[0];
    }
    
    // Sort the array
    bubbleSort(values, count);
    
    // Calculate median
    if (count % 2 == 0) {
        // Even number of elements - return average of two middle elements
        uint16_t mid1 = values[count / 2 - 1];
        uint16_t mid2 = values[count / 2];
        return (mid1 + mid2) / 2;
    } else {
        // Odd number of elements - return middle element
        return values[count / 2];
    }
}

/**
 * Simple bubble sort for float array
 * @param values Array to sort
 * @param count Number of elements
 */
static void bubbleSortFloat(float* values, uint16_t count) {
    for (uint16_t i = 0; i < count - 1; i++) {
        for (uint16_t j = 0; j < count - i - 1; j++) {
            if (values[j] > values[j + 1]) {
                // Swap elements
                float temp = values[j];
                values[j] = values[j + 1];
                values[j + 1] = temp;
            }
        }
    }
}

float calculateMedian(float* values, uint16_t count) {
    // Handle edge cases
    if (values == nullptr || count == 0) {
        return 0.0f;
    }
    
    if (count == 1) {
        return values[0];
    }
    
    // Sort the array
    bubbleSortFloat(values, count);
    
    // Calculate median
    if (count % 2 == 0) {
        // Even number of elements - return average of two middle elements
        float mid1 = values[count / 2 - 1];
        float mid2 = values[count / 2];
        return (mid1 + mid2) / 2.0f;
    } else {
        // Odd number of elements - return middle element
        return values[count / 2];
    }
}