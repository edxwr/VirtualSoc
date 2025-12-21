#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include "../../headers/defines.h"

void handleError(const char* error)
{
    char output[64];
    sprintf(output, "[client] Eroare la %s!\n", error);
    perror(output);
    exit(EXIT_FAILURE);
}

int main(void)
{
    char option[3];
    printf("[client] 1 (local) / 2 (online): ");
    fgets(option, 3, stdin);

    char* endptr;
    int host_select = strtol(option, &endptr, 10);
    if (*endptr != '\n' || host_select < 1 || 2 < host_select)
    {
        printf("Introduceti un argument valid: 1 (local)/2 (online)\n");
        exit(EXIT_FAILURE);
    }

    int sd;
    struct sockaddr_in server;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        handleError("socket()");
    
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = host_select == 1 ? inet_addr(LOCAL_HOST) : inet_addr(SERVER_HOST);
    server.sin_port = htons(PORT);

    if (connect(sd, (struct sockaddr*) &server, sizeof(server)) < 0)
        handleError("connect()");
    
    while (1)
    {
        printf("[client] Introdu o comanda: ");
        char* bufferOut = malloc(BUFFER_SIZE);
        fgets(bufferOut, BUFFER_SIZE, stdin);

        int bufferSize = strlen(bufferOut);
        //printf("[d_client] Buffer Out Size: %d\n", bufferSize);
        if (bufferSize > 0 && bufferOut[bufferSize - 1] == '\n')
            bufferOut[bufferSize - 1] = '\0';
        
        printf("[client] Am trimis comanda: %s\n", bufferOut);
        
        write(sd, &bufferSize, sizeof(int));
        write(sd, bufferOut, bufferSize);
        
        read(sd, &bufferSize, sizeof(int));
        //printf("[d_client] Buffer In Size: %d\n", bufferSize);
        char* bufferIn = malloc(bufferSize);
        read(sd, bufferIn, bufferSize);
        
        printf("\033[H\033[J");
        printf("[client] %s\n", bufferIn);
    }

    close(sd);
}