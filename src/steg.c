#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include "ppmfunctions.h"
#include "functions.h"
#include "sequence.h"

int SEPARATORS[] = {' ', '\n', '\t', '#', EOF};
int NO_OF_SEPARATORS = 5;

int main(int argc, char ** args) {

    char mode;              // first argument either d or e
    char filename1[80];     // first filename argument

    // get first filename argument if provided
    if (argc >= 3) {
        mode = args[1][0];
        strcpy(filename1, "./");
        strcat(filename1, args[2]);
    }
    // otherwise, end program
    else {
        printf("Invalid arguments. Use `steg <mode> <file1> [<file2>]`.\n");
        return 1;
    }

    // get the secret from user
    char * secretInput = getpass("Secret:");
    const int SECRET = atoi(secretInput);

    //// Encrypt mode ////
    if (mode == 'e') {
        char filename2[80];     // second filename argument - for encoding message

        // get file location if provided
        if (argc >= 4) {
            strcpy(filename2, "./");
            strcat(filename2, args[3]);
        }
        // otherwise, end program
        else {
            printf("Must provide filename2 argument.\n");
            return 1;
        }

        // get message from user
        char message[1000];
        printf("Enter message:\n");
        scanf("%[^\n]s", message);

        // calculate message length including null character
        const int MESSAGE_LENGTH = strlen(message) + 1;

        // locate input file
        FILE* fp;
        fp = fopen(filename1, "r");

        // save file contents in PPM structure
        struct PPM sourceFile;
        sourceFile = *getPPM(fp);

        // if file is not of P3 format then end program
        if (sourceFile.format[0] != 'P' || sourceFile.format[1] != '3') {
            printf("File must be of P3 format.\n");
            return 1;
        }

        // encode message
        struct PPM encodedMessage;
        encodedMessage = *encode(&sourceFile, message, MESSAGE_LENGTH, SECRET);

        // write encoded message to file
        fp = fopen(filename2, "w+");
        writeOutputFile(fp, &encodedMessage);

        printf("Message hidden in %s.\n", filename2);
    }

    //// Decrypt mode ////
    else if (mode == 'd') {
        // locate input file
        FILE* fp2;
        fp2 = fopen(filename1, "r");

        // save file contents in PPM structure
        struct PPM messageFile;
        messageFile = *getPPM(fp2);

        // if file is not of P3 format then end program
        if (messageFile.format[0] != 'P' || messageFile.format[1] != '3') {
            printf("File must be of P3 format.\n");
            return 1;
        }

        // decode and display message
        char * decodedMessage;
        decodedMessage = decode(&messageFile, SECRET);
        printf("%s\n", decodedMessage);

    }

    //// Invalid mode - end program ////
    else {
        printf("Invalid arguments. Use `steg <mode> <file1> [<file2>]`.\n");
        return 1;
    }

    return 0;
}
