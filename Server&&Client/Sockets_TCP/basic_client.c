#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#define MAX_SIZE 1024

int main(int argc, char *argv[])
{
    int client;
    char buff[MAX_SIZE];
    struct sockaddr_in client_address;
    int port;
    int read_size;
    if (argc != 3)
    {
        printf("Wrong arguments format");
        exit(0);
    }
    if ((client = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot creat socket");
        exit(1);
    }
    port = atoi(argv[2]);

    if (port < 0 && port > 65535)
    {
        printf("Wrong port format");
        exit(1);
    }
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(argv[1]);
    client_address.sin_port = htons(port);
    if (connect(client, (struct sockaddr *)&client_address, sizeof(client_address)) == -1)
    {
        perror("Connection problems");
        exit(2);
    }
    if ((read_size = (read(client, buff, MAX_SIZE))) == -1)
    {
        perror("Reading problems");
        exit(3);
    }
    for (int i = 0; i < read_size ; i++)
    {
        if (((int)(buff[i]) < 127) && ((int)buff[i] > 32))
        {
            printf("%c", buff[i]);
        }
    }
    if (close(client) == -1)
    {
        perror("Cannot close the client");
        exit(4);
    }
}
