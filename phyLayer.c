// Header files
#include "encDec.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <math.h>

#define size 100000

char store[size];
char binString[size] = ""; // framed to binary
char fileContent[size];    // has framed string 2222.....
char store[size];          // to store string converted framed
char resultInBinary[size] = "";
int loopIter;
int option; // to select option

int main(int argc, char *argv[])
{
    if (strcmp(argv[0], "encode") == 0) // encoding lowerCase frames
    {
        strncpy(fileContent, "", strlen(fileContent));
        strncpy(binString, "", strlen(binString));
        strcpy(fileContent, argv[1]);
        int len = strlen(fileContent);
        characterToBinaryFormat(fileContent, len);
        includeParity();
        printf("1: CRC\t2: Hamming\n");
        scanf("%d", &option); // to choose between CRC and Checksum
        switch (option)
        {
        case 1:
            includeCRC();
            FILE *opt = fopen("option.txt", "w");
            fprintf(opt, "%d", option);
            fclose(opt);
            if (remove("SenderSideHamming.txt") == 0) // remove file if already exists
                printf("File removed");
            break;
        case 2:
            // hamming code
            FILE *hopt = fopen("option.txt", "w");
            fprintf(hopt, "%d", option);
            fclose(hopt);
            includeHammining();
            if (remove("SenderSideCRC.txt") == 0) // remove file if already exists
                printf("File removed");
            break;
        default:
            printf("entered invalid option\n");
            exit(1);
        }
        FILE *bW = fopen("filename.binf", "w");
        fputs(binString, bW);
        fclose(bW);
        printf("------------------ done binf--------------- \n");
    }
    if (strcmp(argv[0], "encode2") == 0)
    {
        strncpy(fileContent, "", strlen(fileContent));
        strncpy(binString, "", strlen(binString));
        strcpy(fileContent, argv[1]);
        int len = strlen(fileContent);
        characterToBinaryFormat(fileContent, len);
        includeParity();
        FILE *bW = fopen("filename.check", "w");
        fputs(binString, bW);
        fclose(bW);
        printf("------------------ done check--------------- \n");
    }
    else if (strcmp(argv[0], "decode") == 0) // decoding lowerCase frames
    {
        printf("------------------ in decode--------------- \n");
        strncpy(resultInBinary, "0", strlen(resultInBinary));
        strcpy(resultInBinary, argv[1]);
        FILE *opR = fopen("option.txt", "r");
        fscanf(opR, "%d", &option);
        fclose(opR);
        printf("%d\n", option);
        if (option == 1)
        {
            if (remove("ReceiverSideHamming.txt") == 0) // remove file if already exists
                printf("File removed");
            removeCRC(resultInBinary);
        }
        else if (option == 2)
        {
            if (remove("ReceiverSideCRC.txt") == 0) // remove file if already exists
                printf("File removed");
            removeHamming(resultInBinary);
        }
        else
        {
            printf("user have entered invalid option, soo stop!!!\n");
            exit(1);
        }
        paritRemoval(resultInBinary);
        binartyToCharacterConvertion(resultInBinary);
        execl("dataLayer", "dF", store, NULL); // execl() call to deframe lowerCase frames
        printf("------------------ done decode--------------- \n");
    }
    else if (strcmp(argv[0], "decode2") == 0) // decoding lowerCase frames
    {
        strncpy(resultInBinary, "0", strlen(resultInBinary));
        strcpy(resultInBinary, argv[1]);
        paritRemoval(resultInBinary);
        binartyToCharacterConvertion(resultInBinary);
        execl("dataLayer", "dF2", store, NULL); // execl() call to deframe lowerCase frames
    }
    else
    {
        printf("phy else\n");
    }
    return 0;
}

// Encoding

void appendSyn() // syn character in binary
{
    int syn = 22;
    int temp[8] = {0};
    int syn1 = 7, syn2 = 15;
    char synString[size] = "0";
    for (int i = 0; i < 8; i++)
    {
        temp[i] = syn % 2;
        syn /= 2;
        synString[syn1--] = temp[i] + '0';
        synString[syn2--] = temp[i] + '0';
    }
    printf("synChar : %s\n", synString);
    strcat(binString, synString);
}

int appendLen(int len, int digit, int index) // converting len in binary
{
    printf("length of frame n digit :%d,%d\n", len, digit);
    int temp[8] = {0};
    char aToi[2];
    if (digit == 2)
    {
        aToi[0] = fileContent[index];
        aToi[1] = fileContent[index + 1];
    }
    else
    {
        aToi[0] = fileContent[index];
        aToi[1] = '\0';
    }
    int digitSize = atoi(aToi);
    printf("atoi:%d\n", digitSize);
    char lenString[8];
    int j = 7;
    for (int i = 0; i < 8; i++)
    {
        temp[i] = digitSize % 2;
        digitSize /= 2;
        lenString[j--] = temp[i] + '0';
    }
    printf("lenstring:%s\n", lenString);
    strcat(binString, lenString);
    printf("after appending digit:\t");
    int l = strlen(binString);
    for (int i = 0; i < l; i++)
    {

        printf("%c", binString[i]);
        if (i % 8 == 7)
            printf(" ");
    }
    printf("\n");
}

void characterToBinaryFormat(char fileContent[], int frameLen) // converting character data in binary format
{
    int curr = 0;
    int startOfData;
    int loop = 0;
    printf("frame length:%d\n", frameLen);
    int iter = 0;
    int t = frameLen;
    int x = frameLen;
    if (x > 38)
    {
        int copy = x;
        while (copy / 38 >= 1)
        {
            printf("copy:%d\n", copy);
            appendSyn();
            int digit = 2;
            appendLen(32, digit, curr + 4);
            startOfData = curr + 6;
            for (int i = startOfData; i < startOfData + 32; i++)
            {
                int value = fileContent[i]; // ascii value
                int temp[8] = {0};
                int iter = 7;
                char dataString[8] = "";
                for (int j = 0; j < 8; j++)
                {
                    temp[j] = value % 2;
                    value /= 2;
                    dataString[iter--] = temp[j] + '0';
                }
                strcat(binString, dataString);
            }

            copy -= 38;
            curr += 38;
            printf("loop iter :\n");
        }
        x = 0;
        printf("curr:%d\n", curr);
    }
    if (x <= 38)
    {
        int frameLen = t % 38;
        printf("t/38:%d\n", frameLen);
        appendSyn();
        if (frameLen < 15)
        {
            int digit = 1;
            appendLen(frameLen, digit, curr + 4);
            startOfData = curr + 5;
        }
        if (frameLen > 15)
        {
            int digit = 2;
            appendLen(frameLen, digit, curr + 4);
            startOfData = curr + 6;
        }
        for (int i = startOfData; i < t; i++)
        {
            int value = fileContent[i]; // ascii value
            int temp[8] = {0};
            int iter = 7;
            char dataString[8] = "";
            for (int j = 0; j < 8; j++)
            {
                temp[j] = value % 2;
                value /= 2;
                dataString[iter--] = temp[j] + '0';
            }
            printf("datas:%s\n", dataString);
            strcat(binString, dataString);
        }
    }

    printf("normal string:\t");
    int l = strlen(binString);
    for (int i = 0; i <= l; i++)
    {

        printf("%c", binString[i]);
        if (i % 8 == 7)
            printf(" ");
    }
    printf("\n");
}

void includeParity() // to include parity
{
    // printf("%d\n",binSize);
    int start = 16;
    int binSize = strlen(binString);
    for (int i = start + 1; i < binSize; i = i + 8)
    {
        int k = i;
        int count = 0;
        while (k % 8 != 0 && k < binSize)
        {
            if (binString[k] == 1 + '0')
                count++;
            k++;
        }
        binString[start] = (count % 2 == 0) ? 1 + '0' : 0 + '0';
        start = k;
    }
    printf("parity:\t");
    int l = strlen(binString);
    for (int i = 0; i < l; i++)
    {

        printf("%c", binString[i]);
        if (i % 8 == 7)
            printf(" ");
    }
    printf("\n%d", l);
    printf("\n");
}
void includeCRC()
{ // to include CRC
    printf("------------------crc-------------------\n");
    int i, j, keylen, msglen;
    bool flag = true;

    char binInputTemp[capacity];
    strcpy(binInputTemp, binString);
    msglen = strlen(binInputTemp);
    char key[capacity] = "100000100110000010001110110110111";
    keylen = strlen(key);
    char temp[capacity], quot[capacity], rem[capacity], key1[capacity];
    i = 0;
    while (i < keylen - 1 && flag)
    {
        binInputTemp[msglen + i] = '0';
        ++i;
    }
    strcpy(key1, key);
    i = 0;
    do
    {
        temp[i] = binInputTemp[i];
        ++i;
    } while (i < keylen && flag);
    i = 0;
    flag = false;
    do
    {
        quot[i] = temp[0];
        if (quot[i] == '0' && !flag)
        {
            int k = 0;
            for (j = 0; j < keylen && !flag; k++, j++)
                key[j] = '0';
        }
        else
        {
            int k = 0;
            if (k == 0)
                strcpy(key, key1);
        }
        j = keylen - 1;
        do
        {
            int k = 0;
            int one = 1, zero = 0;
            if (!flag && k == 0 && temp[j] == key[j])
                rem[j - 1] = zero + '0';
            else
                rem[j - 1] = one + '0';
            j--;
        } while (j > 0 && !flag);
        rem[keylen - 1] = binInputTemp[i + keylen];
        strcpy(temp, rem);
        ++i;
    } while (i < msglen && !flag);

    strcpy(rem, temp);
    strcat(binString, rem);
    printf("----------------after crc:-----------\n");
    printf("rem\n");
    printf("%s\n", rem);
    printf("binString\n");
    printf("%s\n", binString);
    FILE *er = fopen("SenderSideCRC.txt", "w");
    fprintf(er, "%s", binString);
    char z[] = "\nThe Remainder is: \n";
    fprintf(er, "%s", z);
    fprintf(er, "%s", rem);
    fclose(er);
    printf("------------------done crc append-------------------\n");
}
void removeCRC(char resultInBinary[])
{
    printf("------------------remove crc-------------------\n");
    int i, j, keylen, msglen;
    bool flag = true;
    char tempResult[capacity];
    strcpy(tempResult, resultInBinary);
    msglen = strlen(tempResult);
    char key[capacity] = "100000100110000010001110110110111";
    // char key[capacity] = "1001";
    keylen = strlen(key);
    char temp[capacity], quot[capacity], rem[capacity], key1[capacity];
    strcpy(key1, key);
    i = 0;
    flag = true;
    do
    {
        temp[i] = resultInBinary[i];
        ++i;
    } while (i < keylen && flag);
    i = 0;
    flag = false;
    do
    {
        quot[i] = temp[0];
        if (quot[i] == '0' && !flag)
        {
            int k = 0;
            for (j = 0; j < keylen && !flag; ++j, ++k)
                key[j] = '0';
        }
        else
        {
            int k = 0;
            if (k == 0)
                strcpy(key, key1);
        }
        j = keylen - 1;
        do
        {
            int k = 0;
            int zerodigit = 0;
            int onedigit = 1;
            if (!flag && k == 0 && temp[j] == key[j])
                rem[j - 1] = zerodigit + '0';
            else
                rem[j - 1] = '0' + onedigit;
            j--;
        } while (j > 0 && !flag);
        rem[keylen - 1] = resultInBinary[i + keylen];
        strcpy(temp, rem);
        i++;
    } while (i < msglen - keylen + 1 && !flag);
    strcpy(rem, temp);
    printf("reme\n");
    printf("%s\n", rem);
    printf("---------quioyent----------\n");
    printf("%s\n", quot);
    printf("------------resultInBinary------------\n");
    int flagBool = 0;
    int remLen = strlen(rem);
    for (int l = 0; l < remLen; l++)
    {
        if (rem[l] == '1')
        {
            flagBool = 1;
            break;
        }
    }
    if (flagBool == 0)
    {
        printf("no error\n");
        char o[capacity];
        FILE *er = fopen("ReceiverSideCRC.txt", "w");
        char z[] = "The Remainder is: \n";
        fprintf(er, "%s", z);
        fprintf(er, "%s", rem);
        fclose(er);
        i = 0;
        do
        {
            o[i] = resultInBinary[i];
            ++i;
        } while (i < strlen(resultInBinary) - keylen + 1);
        strcpy(resultInBinary, o);
        printf("%s\n", resultInBinary);
        printf("------------resultInBinary------------\n");
    }
    else
    {
        printf("error\n");
    }
    printf("------------------done remove crc-------------------\n");
}
void includeHammining()
{
    printf("------------------adding hamming-------------------\n");
    char temp[capacity];
    strcpy(temp, binString);
    int len = strlen(temp);
    int i, j;
    int k = 0;
    int l = 0;
    int r = 0;
    int top = 0;
    bool flag = true;
    int parity[capacity];
    int tempStore[capacity];
    char result[capacity];
    do
    {
        top++;
        ++r;
        top += pow(2, r);
    } while (pow(2, r) < (len + r + 1) && flag);
    int total = len + r;
    tempStore[total];
    int s = r + 0;
    r = s;
    parity[r];
    i = 0;
    do
    {
        // tempStore[i] = (i==pow(2,k)-1)?0:temp[l]-'0';
        //     if(tempStore[i]==0) k++;
        //     else l++;
        if (i == pow(2, k) - 1)
        {
            tempStore[i] = '0' + 0;
            ++k;
        }
        else
        {
            tempStore[i] = temp[l] - '0';
            ++l;
        }
        ++i;
    } while (i < total && flag);
    i = 0;
    do
    {
        int countnoof = 0;
        j = 0;
        do
        {
            if ((j + 1) & (1 << i))
            {
                countnoof = (tempStore[j] == 1) ? countnoof + 1 : countnoof;
            }
            j++;
        } while (j < total && flag);
        parity[i] = (countnoof % 2 == 0) ? 0 : 1;
        ++i;
    } while (i < r);
    i = 0;
    do
    {
        tempStore[(int)pow(2, i) - 1] = parity[i];
        ++i;
    } while (i < r);
    i = 0;
    do
    {
        result[i] = tempStore[i] + '0';
        ++i;
    } while (i < total);
    result[i] = '\0';
    strcpy(binString, result);
    FILE *er = fopen("SenderSideHamming.txt", "w");
    fprintf(er, "%s", binString);
    for(int z=0;z<r;z++){
        fprintf(er, "\nparity bit :p[%d]:%d",z+1, parity[z]);
    }
    fclose(er);
    printf("------------------done hamming-------------------\n");
}
void removeHamming(char resultBinary[])
{
    printf("------------------remove hamming-------------------\n");
    // remove parity bits
    int len = strlen(resultBinary);
    int i, j = 0;
    char tempStore[capacity];
    int index = 0;
    i = 0;
    do
    {

        if (i == pow(2, j) - 1)
        {
            j++;
        }
        else
        {
            tempStore[index++] = resultBinary[i];
        }
        i++;
    } while (i < len);
    tempStore[index] = '\0';
    strcpy(resultBinary, tempStore);
    // removed parity bits
    FILE *er = fopen("ReceiverSideHamming.txt", "w");
    fprintf(er, "%s", resultBinary);
    fclose(er);
    printf("------------hamming done on resultInBinary------------\n");
}
// Consumer
void checkParity(char str[], int len)

{
    int i = 0;
    int start = 16; // check for more than 1 word
    int inc = 8;
    while (str[start + 1] != '\0')
    {
        int k = start;
        int count = 0;
        while (k % 8 != 0 && k < len)
        {
            if (str[k] == '1')
                count++;
            k++;
        }
        if (count % 2 != 0 && str[start] != 1 || count % 2 == 0 && str[start] == 1)
        {
            printf("\nerror in frame\n");
        }
        start = k++;
    }
    printf("no error\n");
}

void paritRemoval(char result[]) // removing parity
{
    int len = strlen(result);
    // printf("%d",len);
    // checkParity(result,len);
    printf("\nremoving parity: ");
    for (int i = 0; i < len; i++)
    {
        if (i % 8 == 0)
            result[i] = '0';
        printf("%c", result[i]);
        if (i % 8 == 7)
            printf(" ");
    }
}
void convertingAsciiToCharacter(int storeTemp[], int length) // converting ascii to respective character
{
    strncpy(store, "", strlen(store));
    int x = length;
    int newIndex = 0;
    printf("values in :\n");
    for (int i = 0; i < length; i++)
    {
        printf("%d\t", storeTemp[i]);
    }
    printf("\n");
    printf("length:%d\n", x);
    int curr = 0;
    int decCount = 0;
    while (x / 35 >= 0)
    {
        int limit = curr + 35;
        decCount = 0;
        for (int i = curr; i < limit && i < length; i++)
        {
            char check[2] = "";

            if (decCount < 3)
            {
                printf("p:%d\n", storeTemp[i]);
                sprintf(check, "%d", storeTemp[i]);
                printf("check:%s,%d\n", check, decCount);
                int iter = 0;
                while (check[iter] != '\0')
                {
                    store[newIndex] = check[iter];
                    ++iter;
                    ++newIndex;
                }
                ++decCount;
            }
            else
            {
                store[newIndex] = (char)storeTemp[i];
                if (i != length)
                    ++newIndex;
            }
        }
        curr += 35;
        x -= 35;
    }

    store[newIndex] = '\0';
    int p = strlen(store);

    printf("in store :%s\n", store);
}
void binartyToCharacterConvertion(char result[])
{
    int l = strlen(result);

    int newStoreSize = l / 8; // check to odd
    printf("len of binary string:%d\n", l);
    int storeTemp[newStoreSize];

    printf("\nlen :%d\n", newStoreSize);
    int index = 0; // len
    for (int i = 0; i < l; i = i + 8)
    {
        // printf("in");
        int temp[8] = {0};
        int k = 0;
        int power = 7;
        int perChar = 0;
        int start = i;
        int base = 2;

        while (k != 8 && start < l)
        {
            char c = result[start];
            temp[k] = c - '0';
            // printf("d:%d ",temp[k]);
            int p = (int)(pow(base, power) * temp[k]);
            // printf("p:%d ",p);
            perChar += p;
            ++k;
            --power;
            ++start;
        }

        storeTemp[index] = perChar;
        ++index;
    }
    printf("values:\n");
    for (int i = 0; i < newStoreSize; i++)
    {
        printf("%d\t", storeTemp[i]);
    }
    printf("\n");
    convertingAsciiToCharacter(storeTemp, index);
}
// end of ---phyLink---