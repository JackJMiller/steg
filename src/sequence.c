#include <stdio.h>
#include <stdlib.h>
#include "ppmfunctions.h"
#include "sequence.h"
#include "functions.h"

/*
** This generates the pseudo-random sequence of pixels to specify the order in
** which they are manipulated for any given secret seed. The rand() function in
** C is used.
*/
void generatePixelSequence(int * sequence, int imageSize, int length, int seed) {
    srand(seed);        // set seed for rand functions
    int pixelNumber;    // randomly selected pixel

    for (int i = 0; i < length; i++) {
        pixelNumber = getNextPixel(sequence, imageSize, i);
        sequence[i] = pixelNumber;
    }
}

/*
** Gets next pixel in sequence of pixels selected to encode the message.
*/
int getNextPixel(int sequence[], int imageSize, int length) {
    int pixelNumber = rand() % imageSize;                   // compute pixel position to be added to sequence
    while (isInArray(pixelNumber, sequence, length)) {      // while pixel is already in sequence
        pixelNumber = (pixelNumber + 1) % imageSize;        // jump to next pixel position
    }
    return pixelNumber;
}
