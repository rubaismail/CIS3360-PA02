/*============================================================================
| Assignment: pa02 - Calculate the checksum of an input file given:
|              -> the name of the input file,
|              -> the checksum size of either 8, 16, or 32 bits
|
| Author: Ruba Ismail
| Language: c
| To Compile: gcc -o pa02 pa02.c
| To Execute: c -> ./pa02 inputFileName.txt checksumSize
| where inputFilename.txt is the input file
| and checksumSize is either 8, 16, or 32
| Note:
| All input files are simple 8 bit ASCII input
| All execute commands above have been tested on Eustis
|
| Class: CIS3360 - Security in Computing - Spring 2025
| Instructor: McAlpin
| Due Date: 03-23-2025
+===========================================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define LINE_WIDTH 80
#define INITIAL_BUFFER_SIZE 1024

// prints the file contents in rows of 80 characters
void printRows(char *buffer, int length)
{
    for (int i = 0; i < length; i++) {
        putchar(buffer[i]);
        if ((i + 1) % LINE_WIDTH == 0)
            putchar('\n');
    }
    //if (length % LINE_WIDTH != 0)
       //printf("\n");

    // adds a \n only if the last character isnt a \n
    if (length > 0 && buffer[length - 1] != '\n')
        printf("\n");
}


void printXPadding(int currentCount, int paddingNeeded){
    for (int i = 0; i < paddingNeeded; i++)
    {
        printf("X");
        if (((currentCount + i + 1) % LINE_WIDTH) == 0)
            printf("\n");
    }
    if (paddingNeeded > 0 && ((currentCount + paddingNeeded) % LINE_WIDTH) != 0)
        printf("\n");
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s inputFilename checksumSize\n", argv[0]);
        return 1;
    }

    // Validatinng checksum size
    int checkSumSize = atoi(argv[2]);
    if (checkSumSize != 8 && checkSumSize != 16 && checkSumSize != 32) {
        fprintf(stderr, "Valid checksum sizes are 8, 16, or 32\n");
        return 1;
    }

    FILE *textFile = fopen(argv[1], "rb");
    if (textFile == NULL) {
        printf("Error opening plaintext file\n");
        return 1;
    }

    // reads the file into a dynamically allocated  buffer
    int capacity = INITIAL_BUFFER_SIZE;
    int count = 0;
    char *buffer = malloc(capacity);
    if (!buffer) {
        printf("Memory allocation failed\n");
        fclose(textFile);
        return 1;
    }

    int ch;
    while ((ch = fgetc(textFile)) != EOF)
        {
        buffer[count++] = (char) ch;
        if (count >= capacity)
        {
            capacity *= 2;
            char *temp = realloc(buffer, capacity);
            if (!temp)
            {
                printf("Memory reallocation failed\n");
                free(buffer);
                fclose(textFile);
                return 1;
            }
            buffer = temp;
        }
    }

    fclose(textFile);

    // determining grouping size/ padding if necessary
    int groupSize;
    if (checkSumSize == 8)
        groupSize = 1;
    else if (checkSumSize == 16)
        groupSize = 2;
    else
        groupSize = 4;

    int remainder = count % groupSize;
    int paddingNeeded;

    if (remainder == 0)
        paddingNeeded = 0;
    else
        paddingNeeded = groupSize - remainder;

    int totalLength = count + paddingNeeded;

    printf("\n");   // **adding this to match testcases bcos some textfiles have a newline at the begining
    printRows(buffer, count);

    /* if (checkSumSize != 8) {
       printXPadding(count, paddingNeeded);
    }
    */
    printXPadding(count, paddingNeeded);

    // creating a new buffer for checksum calculation
    char *checksumBuffer = malloc(totalLength);
    if (!checksumBuffer) {
        printf("Memory allocation failed\n");
        free(buffer);
        return 1;
    }
    memcpy(checksumBuffer, buffer, count);

    for (int i = 0; i < paddingNeeded; i++) {
        checksumBuffer[count + i] = 'X';
    }

    // calculating the checksum
    unsigned long checksum = 0;
    int i, j;

    for (i = 0; i < totalLength; i += groupSize)
    {
        unsigned long value = 0;
        for (j = 0; j < groupSize; j++) {
            value = (value << 8) | (unsigned char)checksumBuffer[i + j];
        }
        checksum += value;
    }

    // mask off unwanted
    if (checkSumSize == 8)
        checksum &= 0xFF;
    else if (checkSumSize == 16)
        checksum &= 0xFFFF;
    else
        checksum &= 0xFFFFFFFF;

    printf("%2d bit checksum is %8lx for all %4d chars\n", checkSumSize, checksum, totalLength);

    free(buffer);
    free(checksumBuffer);
    return 0;
}

/*=============================================================================
| I Ruba Ismail (ru062473) affirm that this program is
| entirely my own work and that I have neither developed my code together with
| any another person, nor copied any code from any other person, nor permitted
| my code to be copied or otherwise used by any other person, nor have I
| copied, modified, or otherwise used programs created by others. I acknowledge
| that any violation of the above terms will be treated as academic dishonesty.
+=============================================================================*/