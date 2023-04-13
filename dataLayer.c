#include "encDec.h"

char fileInformation[capacity];
char fileContentRead[capacity]; // to read file
char readFileContent[capacity]; // to store data cmmg from phylayer
char data[capacity];            // upper string
char synChar[] = "2222";
char framedString[capacity] = "";
int loopIter = 0; // loop track

int main(int argc, char *argv[])
{

    if (strcmp(argv[0], "cycle1") == 0)
    {
        strncpy(fileInformation, "", strlen(fileInformation));   // to clear the array
        strcpy(fileInformation, argv[1]); // to store the file information
        int len = strlen(fileInformation);
        printf("in datalink :%s,%d\n", fileInformation, len);
        frameContent(fileInformation, len);
        printf("framed string:\n");
        printf("%s\n", framedString);
        printf("going to phylayer\n");
        execl("phyLayer", "encode", framedString, NULL); // execl() call for converting into binary
    }
    else if (strcmp(argv[0], "cycle2") == 0)
    {
        strncpy(fileInformation, "", strlen(fileInformation));// to clear the array
        strcpy(fileInformation, argv[1]);// to store the file information
        int len = strlen(fileInformation);
        printf("in datalink :%s,%d\n", fileInformation, len);
        frameContent(fileInformation, len);
        printf("framed string:\n");
        printf("%s\n", framedString);
        printf("going to phylayer\n");
        execl("phyLayer", "encode2", framedString, NULL); // execl() call for converting into binary
    }
    else if (strcmp(argv[0], "dF") == 0) // deframming lowercase file data
    {
        strncpy(fileContentRead, "", strlen(fileContentRead)); // to clear the array
        strcpy(fileContentRead, argv[1]);// to store the file information
        int len = strlen(fileContentRead);
        printf("in datalink deframe\n");
        printf("data:%s,%d\n", fileContentRead, len);
        deFrame(fileContentRead);
        FILE *wD = fopen("filename.outf", "w");
        fputs(data, wD);
        fclose(wD);
    }
    else if (strcmp(argv[0], "dF2") == 0) // deframming lowercase file data
    {
        strncpy(fileContentRead, "", strlen(fileContentRead));// to clear the array
        strcpy(fileContentRead, argv[1]);// to store the file information
        int len = strlen(fileContentRead);
        printf("in datalink deframe\n");
        printf("data:%s,%d\n", fileContentRead, len);
        deFrame(fileContentRead);
        FILE *wD = fopen("result.txt", "w");
        fputs(data, wD);
        fclose(wD);
    }
    else
    {
        printf("else case \n");
    }
    return 0;
}

// Framing

void addFrame(char fileInformation[], int len) // to add syn and length of data to the frame
{
    int noOfDigit = 0;
    int tempLen = len;
    while (tempLen > 0)
    {
        tempLen /= 10;
        ++noOfDigit;
    }
    char tempLenString[noOfDigit];
    sprintf(tempLenString, "%d", len);
    int lenDataSize = noOfDigit + len;
    char tempString[lenDataSize];
    strcpy(tempString, synChar);
    strcat(tempString, tempLenString);
    strcat(tempString, fileInformation);
    strcat(framedString, tempString);
    // printf("after concant:%s\n", framedString);
}

void frameContent(char fileInformation[], int len) // making of frame
{
    int x; // frame partition
    int start = 0;
    int tempLen = len;
    loopIter = 0;
    if (tempLen > 32)
    {
        x = tempLen;
        while (x / 32 >= 1)
        {
            loopIter++; // loop iteration count
            char temp[32];
            int iter = 0;
            for (iter = 0; iter < 32; iter++)
            {
                temp[iter] = fileInformation[start];
                ++start;
            }
            temp[iter] = '\0';
            addFrame(temp, 32);
            x -= 32;
        }
        tempLen = 0;
    }
    if (tempLen <= 32)
    {
        int iter = 0;
        int rem = len - start;
        char temp[rem];
        for (iter = 0; iter < rem; iter++)
        {
            temp[iter] = fileInformation[start];
            ++start;
        }
        temp[iter] = '\0';
        addFrame(temp, rem);
    }
}
// Deframing

void deFrame(char store[]) // removing syn and length from frame
{
    int len = strlen(store);
    int x = len;
    int curr = 6;
    int iter = 0;
    int o = 1;
    while (x / 38 > 0)
    {
        while (curr < o * 38)
        {
            data[iter] = store[curr];
            iter++;
            curr++;
        }
        x -= 38;
        curr = curr + 6;
        o++;
    }
    int rem = len - curr;
    if (rem < 10)
        curr -= 1;

    while (curr < len)
    {
        data[iter] = store[curr];
        ++iter;
        ++curr;
    }
    printf("data :%s\n", data);
}
//done