#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>

#define capacity 100000

// data Layer
void addFrame(char fileInformation[], int len);
void frameContent(char fileInformation[], int len);
void deFrame(char store[]);
// Physical Layer
void appendSyn();
int appendLen(int len, int digit, int index);
void characterToBinaryFormat(char fileContent[], int frameLen);
void includeParity();
void paritRemoval(char result[]);
void convertingAsciiToCharacter(int storeTemp[], int length);
void binartyToCharacterConvertion(char result[]);
void includeCRC();
void removeCRC(char resultInBinary[]);
void includeHammining();
void removeHamming(char resultInBinary[]);