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

    char buff[13] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\r', '\n'};
    struct sockaddr_in client_address;
    int port;
    int message_size;
    socklen_t size;
    if (argc != 3)
    {
        printf("Wrong arguments format");
        exit(0);
    }
    if ((client = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Cannot creat socket");
        exit(1);
    }
    port = atoi(argv[2]);

    if (port < 0 && port > 65535)
    {
        printf("Wrong port format");
        exit(2);
    }
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(argv[1]);
    client_address.sin_port = htons(port);
    if (sendto(client, (const char *)buff, strlen(buff), 0, (const struct sockaddr *)&client_address, sizeof(client_address)) == -1)
    {
        perror("Cannot send message");
        exit(3);
    }
    if ((message_size = recvfrom(client, (char *)buff, MAX_SIZE, MSG_WAITALL, (struct sockaddr *)&client_address, &size)) == -1)
    {
        perror("There is no messasge");
        exit(4);
    }
    for (int i = 0; i < message_size; i++)
    {
        if ((int)(buff[i]) < 127 && (int)(buff[i]) > 32)
        {
            printf("%c", buff[i]);
        }
    }

    if (close(client) == -1)
    {
        perror("Cannot close the client");
        exit(5);
    }
}
