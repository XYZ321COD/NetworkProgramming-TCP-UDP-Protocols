#include <stdbool.h>
#include <stdio.h>

bool print(const char *buf);
bool print_2(const char *buf);


int main()
{
    char MY_BUFOR[5] = {'A', 'B', 'C', 'D', 'E'};
    bool state;
    state = print(MY_BUFOR);
    state = print_2(MY_BUFOR);
}


bool print(const char *buf)
{
    int i = 0;
    while (*(buf + i) != 0)
    {
        if ((32 <= (*(buf + i))) && ((*(buf + i)) <= 126))
        {
            printf("%c", *(buf + i));
            i++;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool print_2(const char *buf)
{
    const char *p;
    p = buf;
    while (*p != 0)
    {
        printf("%c", *p);
        p++;
    }
    return true;
}
