struct PPM {
    char format[2];         // PPM formating used
    int width;              // width of image in pixels
    int height;             // height of image in pixels
    int size;               // total number of pixels
    int max;                // max colour value for r g b attributes
    int *colourMap;         // colour values
    char comments[10000];   // comments in original file (see below)
};
