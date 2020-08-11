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
    socklen_t size;
    int n_ports;
    int serwer;
    int message_size;
    char message[MAX_SIZE];
    struct sockaddr_in server_address;
    if (argc != 2)
    {
        printf("Wrong arguments format");
        exit(0);
    }
    n_ports = atoi(argv[1]);

    if (n_ports < 0 && n_ports > 65535)
    {
        printf("Wrong port format");
        exit(1);
    }

    if ((serwer = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("Cannot create server");
        exit(2);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    //server_address.sin_addr.s_addr = inet_addr(argv[1]);
    server_address.sin_port = htons(atoi(argv[1]));
    if ((bind(serwer, (const struct sockaddr *)&server_address, sizeof(server_address))) == -1)
    {
        perror("Cannot bind the server");
        exit(3);
    }
    while (true)
    {
        if ((message_size = recvfrom(serwer, (char *)message, MAX_SIZE, MSG_WAITALL, (struct sockaddr *)&server_address, &size)) == -1)
        {
            perror("There is no messasge");
            exit(5);
        }
        for (int i = 0; i < message_size; i++)
        {
            if ((int)(message[i]) < 127 && (int)(message[i]) > 32)
            {
                printf("%c", message[i]);
            }
        }
        if (sendto(serwer, (const char *)message, strlen(message), 0, (const struct sockaddr *)&server_address, size) == -1)
        {
            perror("Cannot send message");
            exit(6);
        }
    }
}
