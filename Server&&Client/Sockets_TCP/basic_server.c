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
    int n_ports;
    int serwer;
    int client;
    const char message[MAX_SIZE] = {'H', 'e', 'l', 'l', 'o', ' ', 'w', 'o', 'r', 'l', 'd', '\r', '\n'};
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

    if (!(serwer = socket(AF_INET, SOCK_STREAM, 0)))
    {
        printf("Cannot create server");
        exit(2);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_port = htons(atoi(argv[1]));
    if ((bind(serwer, (struct sockaddr *)&server_address, sizeof(server_address))) == -1)
    {
        printf("Cannot bind the server");
        exit(3);
    }
    if (listen(serwer, 2) == -1)
    {
        printf("Cannot Listen");
        exit(4);
    }
    while (true)
    {
        if ((client = accept(serwer, NULL, 0)) == -1)
        {
            printf("Cannot accpet next client");
            exit(5);
        }
        if (!(write(client, message, MAX_SIZE)))
        {
            printf("Write Problems");
            exit(6);
        }
    }
}
