#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdint.h>
#define MAX_SIZE 1024
#define MAX_SIZE_SEND 1024
#define ERROR_SIZE 7
#define PORT 2019

int countDigit(int n);
int main(int argc, char *argv[])
{
    bool can_send_message = true;
    bool was_there_r = false;
    bool can_send_error = false;
    int read_size = 1;
    //int n_ports;
    int server;
    int client;
    int sum = 0;
    int sum_digits = 0;
    int number = 0;
    int number_of_white_spaces = 0;
    char buff[MAX_SIZE];
    char buff_send[MAX_SIZE_SEND];
    char ERROR[ERROR_SIZE] = {'E', 'R', 'R', 'O', 'R', '\r', '\n'};
    struct sockaddr_in server_address;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create server");
        exit(2);
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_address.sin_port = htons(PORT);
    if ((bind(server, (struct sockaddr *)&server_address, sizeof(server_address))) == -1)
    {
        perror("Cannot bind the server");
        exit(3);
    }
    if (listen(server, 1) == -1)
    {
        perror("Cannot Listen");
        exit(4);
    }

    while (true)
    {
        if (read_size == -1)
        {
            perror("Read Problems");
            exit(5);
        }
        if (read_size == 0)
        {
            if (close(client) == -1)
            {
                perror("close problems");
                exit(3);
            }
        }
        if ((client = accept(server, NULL, 0)) == -1)
        {
            perror("Cannot accpet next client");
            exit(6);
        }
        can_send_message = true;
        can_send_error = false;
        was_there_r = false;
        while ((read_size = read(client, buff, MAX_SIZE)) > 0)
        {
            /*//////////////////////////////////////////////////////*/
            for (int i = 0; i < read_size; i++)
            {
                if (buff[i] >= 48 && buff[i] <= 57)
                {
                    if ((9 * number + (buff[i] - '0')) > (INT_MAX - number))
                    {
                        if (!can_send_error)
                        {
                            if ((write(client, ERROR, ERROR_SIZE)) == -1)
                            {
                                perror("Write problems");
                                exit(7);
                            }
                        }
                        can_send_error = true;
                        can_send_message = false;
                        was_there_r = false;
                    }
                    number = 10 * number + (buff[i] - '0');
                    number_of_white_spaces = 0;
                    was_there_r = false;
                }

                else if (buff[0] == ' ' && (sum == 0))
                {
                    if (!can_send_error)
                    {
                        if ((write(client, ERROR, ERROR_SIZE)) == -1)
                        {
                            perror("Write problems");
                            exit(8);
                        }
                    }
                    can_send_error = true;
                    can_send_message = false;
                    was_there_r = false;
                    sum = 0;
                    number = 0;
                }

                else if (buff[i] == ' ')
                {
                    if (number_of_white_spaces == 0)
                    {
                        if (sum > (INT_MAX - number))
                        {
                            if (!can_send_error)
                            {
                                if ((write(client, ERROR, ERROR_SIZE)) == -1)
                                {
                                    perror("Write problems");
                                    exit(9);
                                }
                            }
                            can_send_error = true;
                            can_send_message = false;
                        }
                        sum += number;
                        number = 0;
                        number_of_white_spaces++;
                        was_there_r = false;
                    }
                    else
                    {
                        can_send_message = false;
                        was_there_r = false;
                        sum = 0;
                        number = 0;
                        if (!can_send_error)
                        {
                            if ((write(client, ERROR, ERROR_SIZE)) == -1)
                            {
                                perror("Write problems");
                                exit(10);
                            }
                        }
                        can_send_error = true;
                    }
                }
                else if (buff[i] == '\r')
                {
                    if (number_of_white_spaces == 0)
                    {
                        was_there_r = true;
                    }
                    else
                    {
                        was_there_r = true;
                        can_send_message = false;
                        number_of_white_spaces = 0;
                        number = 0;
                        sum = 0;
                        if (!can_send_error)
                        {
                            if ((write(client, ERROR, ERROR_SIZE)) == -1)
                            {
                                perror("Write problems");
                                exit(11);
                            }
                        }
                        can_send_error = true;
                    }
                }
                else if (buff[i] == '\n')
                {
                    if (was_there_r && number_of_white_spaces == 0)
                    {
                        if (sum > (INT_MAX - number))
                        {
                            if (!can_send_error)
                            {
                                if ((write(client, ERROR, ERROR_SIZE)) == -1)
                                {
                                    perror("Write problems");
                                    exit(13);
                                }
                            }
                            can_send_error = true;
                            can_send_message = false;
                            number = 0;
                            sum = 0;
                        }
                        else if (sum == 0 && number == 0)
                        {
                            if (!can_send_error)
                            {
                                if ((write(client, ERROR, ERROR_SIZE)) == -1)
                                {
                                    perror("Write problems");
                                    exit(13);
                                }
                            }
                            can_send_error = true;
                            can_send_message = false;
                            number = 0;
                            sum = 0;
                        }
                        sum += number;
                        sprintf(buff_send, "%d", sum);
                        sum_digits = countDigit(sum);
                        buff_send[sum_digits] = '\r';
                        buff_send[sum_digits + 1] = '\n';
                        if (can_send_message)
                        {
                            if ((write(client, buff_send, sum_digits + 2)) == -1)
                            {
                                perror("Write problems");
                                exit(14);
                            }
                            sum = 0;
                            number = 0;
                            can_send_error = false;
                        }
                        if (!can_send_message)
                        {
                            can_send_message = true;
                            number = 0;
                            sum = 0;
                            was_there_r = false;
                            number_of_white_spaces = 0;
                            can_send_error = false;
                        }
                    }
                    else
                    {
                        sum = 0;
                        number = 0;
                        was_there_r = false;
                        number_of_white_spaces = 0;
                        can_send_message = false;
                        if (!can_send_error)
                        {
                            if ((write(client, ERROR, ERROR_SIZE)) == -1)
                            {
                                perror("Write problems");
                                exit(15);
                            }
                        }
                        can_send_error = true;
                    }
                }
                else
                {
                    number_of_white_spaces = 0;
                    was_there_r = false;
                    can_send_message = false;
                    sum = 0;
                    number = 0;
                    if (!can_send_error)
                    {
                        if ((write(client, ERROR, ERROR_SIZE)) == -1)
                        {
                            perror("Write problems");
                            exit(16);
                        }
                    }
                    can_send_error = true;
                }
            }
        }
    }
}
int countDigit(int n)
{
    int count = 0;
    while (n != 0)
    {
        n = n / 10;
        ++count;
    }
    return count;
}
