#include "encDec.h"

char fileContents[capacity];
char finalData[capacity];
int main()
{
    int socket_desc; //socket descriptor
    struct sockaddr_in server_addr;
    char server_ack[capacity], client_req[capacity];
    //create socket
    // Clean buffers strings:
    memset(server_ack, '\0', sizeof(server_ack)); //fill memory with a constant byte
    memset(client_req, '\0', sizeof(client_req));

    // Create socket:
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);//AF_INET: IPv4, SOCK_STREAM: TCP
    int returnFlag = (socket_desc < 0) ? -1 : 0;  //ternary operator
    if (returnFlag ==-1)
    {
        printf("Unable to create socket\n");
        return -1;
    }

    printf("Socket created successfully\n");

    // Set port and IP the same as server-side:
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(2000);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    // Send connection request to server:
    if (connect(socket_desc, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        printf("Unable to connect problem in socket\n");
        return -2;
    }
    printf("Connected with server successfully\n");

    int forkId = fork();    //forking the process

    if (forkId == 0) // encoding process
    {
        printf("Child process\n");
        // child
        FILE *fileRead = fopen("intext.txt", "r"); // read the file
        char c;
        int i = 0;
        while ((c = fgetc(fileRead)) != EOF)
        {
            fileContents[i++] = c;
        }
        fileContents[i] = '\0'; // has the file content
        int z = strlen(fileContents);
        printf("%s\n", fileContents);
        printf("Length of fileContents: %d\n", z);
        int forkId2 = fork();
        if (forkId2 == 0)
        {
            // child
            execl("dataLayer", "cycle1", fileContents, NULL); // calls dataLayer
        }
        else
        {
            // parent
            wait(NULL);
            printf("Child Parent process 2\n");

            char inputFileData[capacity];
            FILE *rB = fopen("filename.binf", "r"); // file read
            char readChar;
            int i = 0;
            while ((readChar = fgetc(rB)) != EOF)
            {
                inputFileData[i++] = readChar;
            }
            inputFileData[i] = '\0';
            fclose(rB); // close the file
            // Get input from the user:
            strcpy(client_req, inputFileData);

            // Send the message to server:
            if (send(socket_desc, client_req, strlen(client_req), 0) < 0)
            {
                printf("Unable to send message to server!\n");
                return -1;
            }

            // Receive the server's response:
            if (recv(socket_desc, server_ack, sizeof(server_ack), 0) < 0)
            {
                printf("Error while receiving server's msg in client! \n");
                return -1;
            }

            printf("Server's response is: %s\n", server_ack);

            // Close the socket:
        }
    }

    else // decoding process
    {
        wait(NULL); // wait for the child process to finished
        printf("Parent process\n");
        int clientParentfork = fork();
        if (clientParentfork == 0)
        {
            char msg[capacity];
            if (recv(socket_desc, msg, sizeof(msg), 0) < 0)
            {
                printf("Error while receiving server's msg!\n");
                return -1;
            }
            else
            {
                printf("recienved data: %s\n", msg);
            }
            printf("Server's final response: %s:", server_ack);
            strcpy(client_req, "............commmunication done........\n");
            if (send(socket_desc, client_req, strlen(client_req), 0) < 0)
            {
                printf("Unable to send message problem in sockets!\n");
                return -1;
            }
            execl("phyLayer", "decode2", msg, NULL); // calls phyLayer
        }
        else
        {
            wait(NULL);
            char msgq[capacity];
            FILE *rB = fopen("result.txt", "r"); // file read
            char readChar;
            int i = 0;
            while ((readChar = fgetc(rB)) != EOF)
            {
                msgq[i++] = readChar;
            }
            msgq[i] = '\0';
            fclose(rB);
            printf("-----------------data from server sockect:--------------\n");
            printf("%s\n", msgq);
        }
    }
    close(socket_desc);
    return 0;
}