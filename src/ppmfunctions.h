#include "PPM.h"
struct PPM *getPPM(FILE *f);
struct PPM *encode(struct PPM * im, char * message, unsigned int mSize, unsigned int secret);
char *decode(struct PPM * im, unsigned int secret);
char getNextEncodedChar(int * colourMap, int imageSize, int * pixelSequence);
void showPPM(struct PPM *image);
void clonePPM(struct PPM *destination, struct PPM *source);
void modifyLeastSigBits(struct PPM * file, char * message, int mSize, int * pixelSequence);
void writeOutputFile(FILE* f, struct PPM *file);
int readNextValue(FILE *fp, char value[], char comments[]);
void readAllValues(FILE *fp, int *image, char comments[]);
void recordComment(char comments[], char comment[], int position);
int getSegment(char segments[], int index, char segment[]);
int getNextComment(char comments[], char comment[]);
int addSegment(char array[], int length, char segment[]);
