#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>


int main(int argc, char *argv[])
{
    int coppied;
    int new;
    char buf[1];
    int r_read = 0;

    if (argc != 3)
    {
        printf("Wrong input - Example input ./... file1 file2");
        exit(0);
    }
    else
    {
        coppied = open(argv[1], O_RDONLY | O_CREAT);
        new = open(argv[2], O_WRONLY | O_CREAT);
        if (coppied == -1 || new == -1)
        {
            perror("Program");
        }
        else
        {
            while (r_read = read(coppied, buf, 1), r_read > 0)
            {
                char *write_pointer = buf;
                int r_write = 0;
                while (r_write = write(new, write_pointer, 1))
                {
                    if(r_write == -1) {
                        perror("Program");
                    }
                    if(r_write == 0) {
                        break;
                    }
                    r_read -=r_write;
                    if(r_read ==0 ) break;
                }
                if (r_read == -1)
                {
                    perror("Program");
                }
            }
        }
        if (close(coppied) < 0 && close(new) < 0)
        {
            perror("Program");
        }
    }
}
