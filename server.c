#include "encDec.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

char clientSentData[capacity];
pthread_mutex_t mutex; // mutex lock variable

sem_t fullA, fullE, fullI, fullO, fullU, fullDigit, fullWrite;           // semapore full variables
pthread_t thread1, thread2, thread3, thread4, thread5, thread6, thread7; // thread variables
sem_t emptyA, emptyE, emptyI, emptyO, emptyU, emptyDigit, emptyWrite;    // semapore empty variables

int sum = 0;
char changedData[capacity];
char u[capacity];
char charAQueue[10] = {'\0'};      // charAQueue
char charEQueue[10] = {'\0'};      // charEQueue
char charIQueue[10] = {'\0'};      // charIQueue
char charOQueue[10] = {'\0'};      // charOQueue
char charUQueue[10] = {'\0'};      // charUQueue
char digitCountQueue[10] = {'\0'}; // digitCountQueue

int Apush = 0, Epush = 0, Ipush = 0, Opush = 0, Upush = 0, Digitpush = 0; // push variables

int iValue(int i) // updates i value
{
    return (i == 10) ? 0 : i;
}
int pushValue(int Value) // updates push value
{
    return (Value == 10) ? 0 : Value;
}
void *charA(char *str) // uppercase A
{
    // printf("string:%s", str);
    // printf("charA\n");
    int len = strlen(str);
    int i = 0;
    do
    {
        sem_wait(&emptyA);          // wait for emptyA
        pthread_mutex_lock(&mutex); // lock mutex
        char up = (str[i] == 'a') ? toupper(str[i]) : str[i];
        Apush = pushValue(Apush);
        charAQueue[Apush++] = up;
        pthread_mutex_unlock(&mutex); // unlock mutex
        sem_post(&fullA);             // post fullA
        i++;
    } while (i < len);
}

void *charE(int *len) // uppercase E
{
    int i = 0;
    int iter = 1;
    do
    {
        sem_wait(&fullA);           // wait for fullA
        sem_wait(&emptyE);          // wait for emptyE
        pthread_mutex_lock(&mutex); // lock mutex
        i = iValue(i);
        // printf("i:%d\n", i);
        char up = (charAQueue[i] == 'e') ? toupper(charAQueue[i]) : charAQueue[i];
        Epush = pushValue(Epush);
        charEQueue[Epush++] = up;
        // printf("Epush:%d", Epush);
        // printf("values in E:%c\n", charEQueue[Epush]);
        ++i;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyA); // post emptyA
        sem_post(&fullE);  // post fullE
        ++iter;
    } while (iter < *len + 1);
}
void *charI(int *len) // uppercase I
{
    int i = 0;
    int iter;
    for (iter = 2; iter < *len + 2; ++iter)
    {
        sem_wait(&fullE);
        sem_wait(&emptyI);
        pthread_mutex_lock(&mutex);
        i = iValue(i);
        // printf("i:%d\n", i);
        char up = (charEQueue[i] == 'i') ? toupper(charEQueue[i]) : charEQueue[i]; // uppercase I
        Ipush = pushValue(Ipush);
        charIQueue[Ipush++] = up;
        // printf("Ipush:%d", Ipush);
        // printf("values in I:%c\n", charIQueue[Ipush]);
        ++i;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyE);
        sem_post(&fullI);
        //
    }
}
void *charO(int *len) // uppercase O
{
    int i = 0;
    int iter = 1;
    int end = 1;
    do
    {
        sem_wait(&fullI);
        sem_wait(&emptyO);
        pthread_mutex_lock(&mutex);
        i = iValue(i);
        // printf("i:%d\n", i);
        char up = (charIQueue[i] == 'o') ? toupper(charIQueue[i]) : charIQueue[i];
        Opush = pushValue(Opush);
        charOQueue[Opush++] = up;
        // printf("Opush:%d\t", Opush);
        // printf("values in O:%c\n", charOQueue[Opush]);
        ++i;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyI);
        sem_post(&fullO);
        ++iter;
    } while (iter < *len + end);
}
void *charU(int *len) // uppercase U
{
    int i = 0;
    int iter = 0;
    do
    {
        sem_wait(&fullO);
        sem_wait(&emptyU);
        pthread_mutex_lock(&mutex);
        i = iValue(i);
        // printf("i:%d\n", i);
        char up = (charOQueue[i] == 'u') ? toupper(charOQueue[i]) : charOQueue[i]; // uppercase U
        Upush = pushValue(Upush);
        charUQueue[Upush++] = up;
        // printf("Upush:%d\t", Upush);
        // printf("values in U:%c\n", charUQueue[Upush]);
        i++;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyO);
        sem_post(&fullU);
        ++iter;
    } while (iter < *len);
}
void *digitCount(int *len)
{
    int i = 0;
    int iter = 0;
    do
    {
        sem_wait(&fullU);           // wait for fullU
        sem_wait(&emptyDigit);      // wait for emptyDigit
        pthread_mutex_lock(&mutex); // lock mutex
        i = iValue(i);
        // printf("i:%d\n", i);
        Digitpush = pushValue(Digitpush);
        digitCountQueue[Digitpush] = charUQueue[i];
        char ele = digitCountQueue[Digitpush++];
        sum = (isdigit(ele)) ? sum + (ele - '0') : sum; // sum of digits
        // printf("%c", digitCountQueue[Digitpush]);
        ++i;
        pthread_mutex_unlock(&mutex);
        sem_post(&emptyU);
        sem_post(&fullDigit); // post fullDigit
        ++iter;
        //
    } while (iter < *len);
}
void *dataWrite(int *len)
{
    int i = 0;
    int iter = -1;
    int index = 0;
    do{sem_wait(&fullDigit);
        pthread_mutex_lock(&mutex);
        i = iValue(i);
        // printf("i:%d\n", i);
        // printf("%c", digitCountQueue[i]);
        changedData[index++] = digitCountQueue[i++];// write to file
        pthread_mutex_unlock(&mutex);// unlock mutex
        sem_post(&emptyDigit);// post emptyDigit
        ++iter;
        }while(iter < *len-1);
}
void semaporeCreateFull()
{
    sem_init(&fullA, 0, 0);
    sem_init(&fullE, 0, 0);
    sem_init(&fullI, 0, 0);
    sem_init(&fullO, 0, 0);
    sem_init(&fullU, 0, 0);
    sem_init(&fullDigit, 0, 0);
    sem_init(&fullWrite, 0, 0);
}
void semaporeCreateEmpty()
{
    sem_init(&emptyA, 0, 10);
    sem_init(&emptyE, 0, 10);
    sem_init(&emptyI, 0, 10);
    sem_init(&emptyO, 0, 10);
    sem_init(&emptyU, 0, 10);
    sem_init(&emptyDigit, 0, 10);
    sem_init(&emptyWrite, 0, 10);
}
void semaporeCreate()
{
    semaporeCreateFull();
    semaporeCreateEmpty();
}

void threadCreate(char *str, int *length)
{

    pthread_create(&thread1, NULL, &charA, (void *)str);
    pthread_create(&thread2, NULL, &charE, (void *)length);
    pthread_create(&thread3, NULL, &charI, (void *)length);
    pthread_create(&thread4, NULL, &charO, (void *)length);
    pthread_create(&thread5, NULL, &charU, (void *)length);
    pthread_create(&thread6, NULL, &digitCount, (void *)length);
    pthread_create(&thread7, NULL, &dataWrite, (void *)length);
}
void threadJoin()
{
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    pthread_join(thread3, NULL);
    pthread_join(thread4, NULL);
    pthread_join(thread5, NULL);
    pthread_join(thread6, NULL);
    pthread_join(thread7, NULL);
}
void semaporeFullDestroy()
{
    sem_destroy(&fullA);
    sem_destroy(&fullE);
    sem_destroy(&fullI);
    sem_destroy(&fullO);
    sem_destroy(&fullU);
    sem_destroy(&fullDigit);
    sem_destroy(&fullWrite);
}
void semaporeEmptyDestroy()
{
    sem_destroy(&emptyA);
    sem_destroy(&emptyE);
    sem_destroy(&emptyI);
    sem_destroy(&emptyO);
    sem_destroy(&emptyU);
    sem_destroy(&emptyDigit);
    sem_destroy(&emptyWrite);
}
void semaporeDestroy()
{
    semaporeFullDestroy();
    semaporeEmptyDestroy();
}

// main function
int main(void)
{
    
    struct sockaddr_in server_addr, client_addr;
    char server_ack[capacity], client_req[capacity];

    // Clean buffers:
    memset(server_ack, '\0', sizeof(server_ack));
    memset(client_req, '\0', sizeof(client_req));

    // Create socket:
    int socket_desc, client_sock, client_size;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc < 0)
    {
        printf("Error while creating socket at server side\n");
        return -1;
    }
    printf("server side Socket created successfully\n");

    // Set port and IP:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind to the set port and IP:
    if (bind(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Couldn't bind to the port at server side\n");
        return -1;
    }
    printf("Done with binding at server side\n");

    // Listen for clients:
    if (listen(socket_desc, 1) < 0)
    {
        printf("Error while listening at server side\n");
        return -1;
    }
    printf("\nListening for incoming connections.....\n");

    // Accept an incoming connection:
    client_size = sizeof(client_addr);
    client_sock = accept(socket_desc, (struct sockaddr *)&client_addr, &client_size);

    if (client_sock < 0)
    {
        printf("Can't accept\n");
        return -1;
    }
    printf("Client connected at IP: %s and port: %i\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    int serverFork = fork();
    if (serverFork == 0)
    {
        int childFork = fork();
        if (childFork == 0)
        {
            // Receive client's message:
            if (recv(client_sock, client_req, sizeof(client_req), 0) < 0)
            {
                printf("Couldn't receive erro in sockets\n");
                return -1;
            }
            printf("Msg from client is: %s\n", client_req);
            strcpy(clientSentData, client_req);
            strcpy(server_ack, "This is the server's message is:");

            if (send(client_sock, server_ack, strlen(server_ack), 0) < 0)
            {
                printf("Can't send\n");
                return -1;
            }

            printf("in server data\n");
            printf("%s\n", clientSentData);
            execl("phyLayer", "decode", clientSentData, NULL); // decode
        }
        else
        {
            wait(NULL);
            printf("server child parent process\n");
            char string[capacity];
            FILE *r = fopen("filename.outf", "r");
            char c;
            int i = 0;
            while ((c = fgetc(r)) != EOF)
            {
                string[i] = c;
                i++;
            }
            string[i] = '\0';
            char *str = string;
            int len = strlen(string);
            int *length = &len;

            semaporeCreate();
            pthread_mutex_init(&mutex, NULL); // initialize mutex
            threadCreate(str, length);
            threadJoin();
            pthread_mutex_destroy(&mutex); // destroy mutex
            semaporeDestroy();

            char sentence[] = "\nThe sum of the digits is: ";
            strcat(changedData, sentence);
            char sumString[10];
            sprintf(sumString, "%d", sum);
            strcat(changedData, sumString);
            printf("%s\n", changedData);
            FILE *w = fopen("changedData.txt", "w");
            fprintf(w, "%s", changedData);
            fclose(w);
            printf("--------------------------------------------\n");
        }
    }
    else
    {
        wait(NULL);
        printf("server parent process\n");

        int parentChildfork = fork();
        if (parentChildfork == 0)
        {
            printf("in parent child process\n");
            FILE *rB = fopen("changedData.txt", "r"); // file read
            char readChar;
            int i = 0;
            while ((readChar = fgetc(rB)) != EOF)
            {
                u[i++] = readChar;
            }
            u[i] = '\0';
            fclose(rB);
            execl("dataLayer", "cycle2", u, NULL);
        }
        else
        {
            wait(NULL);
            printf("okook\n");
            FILE *rB = fopen("filename.check", "r"); // file read
            char readChar;
            int i = 0;
            while ((readChar = fgetc(rB)) != EOF)
            {
                u[i++] = readChar;
            }
            u[i] = '\0';
            fclose(rB);
            if (send(client_sock, u, strlen(u), 0) < 0)
            {
                printf("Can't send\n");
                return -1;
            }
            else
            {
                printf("sent\n");
            }
            if (recv(client_sock, client_req, sizeof(client_req), 0) < 0)
            {
                printf("Couldn't receive\n");
                return -1;
            }
            else
            {
                printf("received\n");
            }
            printf("Msg from client: %s\n", client_req);
        }
    }
    // Closing the socket:
    close(client_sock);
    close(socket_desc);
    return 0;
}
// end of main function