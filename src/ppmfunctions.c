#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ppmfunctions.h"
#include "sequence.h"
#include "functions.h"

extern int SEPARATORS[];
extern int NO_OF_SEPARATORS;

/*
** Displays PPM specification corresponding to PPM struct including comments.
** Pixel colours are displayed on separate lines.
*/
void showPPM(struct PPM * image) {
    printf("PPM IMAGE:\n");

    // display format, width, height, and max colour value
    printf("P3\n%d %d\n%d\n", image->width, image->height, image->max);

    // calculate total number of colour attributes - r, g, b
    int totalColourAttributes = 3 * image->size;

    // display each colour
    for (int i = 0; i < totalColourAttributes; i += 3) {
        printf("%d\t%d\t%d\n", image->colourMap[i], image->colourMap[i+1], image->colourMap[i+2]);
    }

    printf("\n");
}

/*
** Returns PPM struct specified by contents of file passed as parameter.
*/
struct PPM * getPPM(FILE * f) {
    struct PPM file;

    // temp string to store colour values
    char value[20];

    // get ppm format
    readNextValue(f, value, file.comments);

    file.format[0] = value[0];
    file.format[1] = value[1];

    // get width of image
    readNextValue(f, value, file.comments);
    file.width = atoi(value);

    // get height of image
    readNextValue(f, value, file.comments);
    file.height = atoi(value);

    // calculate total number of pixels in image
    file.size = file.width * file.height;

    // get max colour value
    readNextValue(f, value, file.comments);
    file.max = atoi(value);

    // array containing every colour value
    file.colourMap = (int *) malloc(sizeof(int) * file.size * 3);

    readAllValues(f, file.colourMap, file.comments);

    return &file;
}

/*
** Encodes message in PPM file 'im' using specified seed 'secret'.
*/
struct PPM * encode(struct PPM * im, char * message, unsigned int mSize, unsigned int secret) {
    // image containing hidden message
    struct PPM resultingImage;

    // copy contents of original image into image to be encoded
    clonePPM(&resultingImage, im);

    int pixelSeqLength = ceil((float) mSize * 8 / 3);

    // generate pixel sequence
    int * pixelSequence;
    pixelSequence = malloc(sizeof(int) * pixelSeqLength);
    generatePixelSequence(pixelSequence, im->size, pixelSeqLength, secret);

    // modify LSB's of image to encode message
    modifyLeastSigBits(&resultingImage, message, mSize, pixelSequence);

    return &resultingImage;
}

/*
** Returns sequence of characters encoded using seed secret in file 'im'.
*/
char * decode(struct PPM * im, unsigned int secret) {
    // allocate decoded message
    char * message;
    message = malloc(sizeof(char) * 1000);

    // allocate pixel sequence
    int * pixelSequence;
    pixelSequence = malloc(sizeof(int) * 2667);

    // initialize seed for pixel sequence
    srand(secret);

    int charIndex = 0;
    char c;

    // add every character up to the null character hidden in file to message
    while ((c = getNextEncodedChar(im->colourMap, im->size, pixelSequence)) != '\0') {
        message[charIndex++] = c;
    }

    message[charIndex] = '\0';

    return message;
}

/*
** Returns next character encoded in colour map passed.
*/
char getNextEncodedChar(int * colourMap, int imageSize, int * pixelSequence) {
    static int pixelIndex = 0;      // index of current pixel in sequence
    static int colourIndex = 0;     // current colour attribute being manipulated
    static int pixel;               // position of current pixel in file

    // get first pixel position and add to sequence
    if (pixelIndex == 0) {
        pixel = getNextPixel(pixelSequence, imageSize, pixelIndex++);
        pixelSequence[0] = pixel;
    }

    int currentColour;              // position of current colour value in file
    bool encoding[8];               // bit strings extracted from file
    int bit;                        // bit extracted from file

    // get next eight bits hidden in file
    for (int bitIndex = 0; bitIndex < 8; bitIndex++) {

        // get position of colour to be read
        currentColour = 3 * pixel + colourIndex;

        // get least significant bit of that colour
        bit = getLastBit(colourMap[currentColour]);

        // record bit found
        encoding[bitIndex] = bit;

        // if end of pixel has been reached
        if (++colourIndex == 3) {
            // get next pixel
            pixel = getNextPixel(pixelSequence, imageSize, pixelIndex);
            pixelSequence[pixelIndex++] = pixel;

            // return to red attribute
            colourIndex = 0;
        }
    }

    // return character with encoding
    return getChar(encoding);;
}

/*
** Modifies least significant bits of colours in file to encode message.
*/
void modifyLeastSigBits(struct PPM * file, char * message, int mSize, int * pixelSequence) {
    char c;                         // character extracted from message
    int pixelSeqIndex = 0;          // index in pixel sequence
    int pixel = pixelSequence[0];   // pixel number
    int currentColour = 0;          // current colour being manipulated in image - r g b
    int colourIndex = 0;            // location of current colour attribute in wider file
    int colour;                     // value of colour to be manipulated
    bool encoding[8];               // binary encoding of character
    bool bit = 0;                   // bit in binary encoding

    for (int charIndex = 0; charIndex < mSize; charIndex++) {
        // get next character and its encoding
        c = message[charIndex];
        getCharEncoding(encoding, c);

        for (int bitIndex = 0; bitIndex < 8; bitIndex++) {  // iterate through bits of character
            bit = encoding[bitIndex];                       // get bit
            colourIndex = 3 * pixel + currentColour;        // get position of colour to be modified
            colour = file->colourMap[colourIndex];          // get colour
            colour = setLastBit(colour, bit);               // alter least significant bit
            file->colourMap[colourIndex] = colour;          // replace colour with resulting value

            if (++currentColour == 3) {                     // if all colour attributes have been modified
                pixel = pixelSequence[++pixelSeqIndex];     // get next pixel position to be modified
                currentColour = 0;                          // return to red attribute
            }
        }
    }
}

/*
** Copy data from one PPM file to another. Contents of 'source' are copied to
** 'destination'.
*/
void clonePPM(struct PPM * destination, struct PPM * source) {
    // transfer properties from original image
    destination->width = source->width;
    destination->height = source->height;
    destination->size = source->size;
    destination->max = source->max;

    // calculate total number of colour attributes - r, g, b
    int totalColourAttributes = 3 * destination->size;

    // array containing every colour value
    destination->colourMap = (int *) malloc(sizeof(int) * totalColourAttributes);

    // copy colour attributes to new file
    for (int i = 0; i < totalColourAttributes; i++) {
        destination->colourMap[i] = source->colourMap[i];
    }
}

/*
** Write to external file the specifications for a PPM image as defined by a PPM
** struct.
*/
void writeOutputFile(FILE * f, struct PPM * file) {

    // get first comment
    char commentText[1000];
    int commentPosition = getNextComment(file->comments, commentText);

    char* textOutput;  // string output

    //// specify PPM version including comments ////
    while (commentPosition == 0) {
        asprintf(&textOutput, "%s\n", commentText);
        fputs(textOutput, f);
        commentPosition = getNextComment(file->comments, commentText);
    }
    asprintf(&textOutput, "P3\n");
    fputs(textOutput, f);


    //// specify width including comments ////
    while (commentPosition == 1) {
        asprintf(&textOutput, "%s\n", commentText);
        fputs(textOutput, f);
        commentPosition = getNextComment(file->comments, commentText);
    }
    asprintf(&textOutput, "%d\n", file->width);
    fputs(textOutput, f);


    //// specify height including comments ////
    while (commentPosition == 2) {
        asprintf(&textOutput, "%s\n", commentText);
        fputs(textOutput, f);
        commentPosition = getNextComment(file->comments, commentText);
    }
    asprintf(&textOutput, "%d\n", file->height);
    fputs(textOutput, f);


    //// specify max color value including comments ////
    while (commentPosition == 3) {
        asprintf(&textOutput, "%s\n", commentText);
        fputs(textOutput, f);
        commentPosition = getNextComment(file->comments, commentText);
    }
    asprintf(&textOutput, "%d\n", file->max);
    fputs(textOutput, f);


    //// write colour map including comments ////

    int totalColourAttributes = 3 * file->size;

    // iterate through pixels
    for (int i = 0; i < totalColourAttributes; i++) {

        // if comment appears before value, write comment then get next one
        while (commentPosition == i + 4) {
            asprintf(&textOutput, "%s\n", commentText);
            fputs(textOutput, f);
            commentPosition = getNextComment(file->comments, commentText);
        }

        // write color value
        asprintf(&textOutput, "%d\n", file->colourMap[i]);
        fputs(textOutput, f);
    }

    //// include any comments at bottom of file ////
    while (commentPosition == totalColourAttributes + 4) {
        asprintf(&textOutput, "%s\n", commentText);
        fputs(textOutput, f);
        commentPosition = getNextComment(file->comments, commentText);
    }

    fclose(f);
}


/*
** Read contents of file starting from current file pointer. To be called after
** reading property header.
*/
void readAllValues(FILE *fp, int * image, char comments[]) {
    int valueIndex = 0;
    int colourValue;
    char value[100];

    // read and store colour values
    while (readNextValue(fp, value, comments)) {
        colourValue = atoi(value);
        image[valueIndex++] = colourValue;
    }

    // add footer
    recordComment(comments, "# F28SD Coursework 2 Output File\0", valueIndex + 4);
}


/*
** Gets the next value in the file with values referring to text between spaces
** and newlines apart from comments which are treated as a single value.
**
** Returns 0 if end of file has been reached and no value to return.
*/
int readNextValue(FILE *fp, char value[], char comments[]) {

    static int valueCount = 0;

    int index;
    char c;
    char comment[200];

    start:
    index = 0;

    // skip all spaces and newlines
    while ((c = fgetc(fp)) == ' ' || c == '\n') {}

    // if array is comment, record remainder of line as comment
    if (c == '#') {
        while (c != '\n') {
            value[index++] = c;
            c = fgetc(fp);
        }

        value[index] = '\0';
        recordComment(comments, value, valueCount);
        goto start;
    }
    // otherwise get next colour value
    else {
        // get each consecutive digit and add to return value
        while (!isInArray(c, SEPARATORS, NO_OF_SEPARATORS)) {
            value[index++] = c;
            c = fgetc(fp);
        }

        value[index] = '\0';
        valueCount++;
    }

    // return 0 if no value found i.e. end of file
    return c != EOF;
}

/*
** Given the comment text and its position in file, this function will record
** appropriately record the comment in the given PPH files' string of comments.
*/
void recordComment(char comments[], char comment[], int pos) {
    static int length = 0;

    // convert position number to string
    char position[20];
    itoa(position, pos);

    // add comment and its position to string of comments
    length = addSegment(comments, length, position);
    length = addSegment(comments, length, comment);
}

/*
** Add segment to string of segments of given length. Start from previous end
** of list and specify new end with EOF.
*/
int addSegment(char array[], int length, char segment[]) {
    int j = 0;      // index within segment
    char c;         // character extracted from segment

    // add each character from segment to array of segments, including null
    while ((c = segment[j++]) != '\0') {
        array[length++] = c;
    }
    array[length++] = '\0';

    // denote end of last segment
    array[length] = EOF;

    return length;
}

/*
** Gets segment at index and stores in `segment`.
** number of characters from index to next null character.
*/
int getSegment(char segments[], int index, char segment[]) {
    int j = 0;      // index within current comment
    char c;         // character extracted from comment

    // store each character in string, including null
    while ((c = segments[index++]) != '\0' && c != EOF) {
        segment[j++] = c;
    }
    segment[j] = '\0';

    return j;       // length of comment
}

/*
** Lazy iterator - iterates through every comment found recorded in `comments`.
** Returns position of comment and stores comment text in `comment`.
*/
int getNextComment(char comments[], char comment[]) {
    static int segmentIndex = 0;    // leftmost index of next comment to return

    // get position of comment
    char commentPosition[21];
    segmentIndex += 1 + getSegment(comments, segmentIndex, commentPosition);

    // get comment contents
    segmentIndex += 1 + getSegment(comments, segmentIndex, comment);

    return atoi(commentPosition);   // comment position as integer
}
