#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "functions.h"

/*
** Returns the resulting value when subject has last bit set to bit.
*/
int setLastBit(unsigned subject, bool bit) {
    return (subject >> 1 << 1) | bit;
}

/*
** Gets last bit of integer value.
*/
bool getLastBit(unsigned value) {
    return (value & 1);
}

/*
** Construct character encoding of given char. Resulting encoding is written to
** given array 'encoding'.
*/
void getCharEncoding(bool encoding[], char character) {
    int a;

    for (int i = 7; i >= 0; i--) {  // iterate through
        a = (character >> i) & 1;   // get char at index i
        encoding[7-i] = a;          // record char in encoding
    }
}

/*
** Returns char with byte encoding.
*/
char getChar(bool byte[]) {
    int output = 0;

    // accumulate bits to result in int value
    for (int i = 0; i < 8; i++) {
        output = output * 2 + byte[i];
    }

    // return character responding to int value
    return output;
}

/*
** Checks if integer element is in array. Returns 1 if present and 0 otherwise.
*/
int isInArray(int element, int array[], int length) {
    for (int i = 0; i < length; i++) {      // iterate through each element
        if (array[i] == element)            // if current element matches target
            return 1;                       // element is present
    }
    // element not found
    return 0;
}

/*
** Produces the string corresponding to the given int. The result is stored in
** the string passed.
*/
void itoa(char string[], int number) {
    // special case
    if (number == 0) {
        string[0] = '0';
        string[1] = '\0';
        return;
    }

    // get string corresponding to reverse number
    int i = 0;
    while (number != 0) {                   // repeat until all digits have been removed
        string[i++] = '0' + (number % 10);  // get last digit
        number /= 10;                       // remove last digit
    }
    string[i] = '\0';

    // reverse string given length i
    for (int j = 0; j < i/2; j++) {
        // swap with mirrored character
        char aux = string[j];
        string[j] = string[i - j - 1];
        string[i - j - 1] = aux;
    }
}
