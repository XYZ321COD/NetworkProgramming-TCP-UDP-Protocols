#include <stdbool.h>
#include <stdio.h>
#define BUFORSIZE 5
bool print(const void *buf, int len);
bool print2(const void *buf, int len);


int main()
{
    char MY_BUFOR[BUFORSIZE] = {'A', 'B', 'C', 'D', 'E'};
    bool state;
    state = print(MY_BUFOR,BUFORSIZE);
    state = print2(MY_BUFOR,BUFORSIZE);
}

bool print(const void *buf, int len)
{
    const char *temporary = (const char *)buf;
    for (int i = 0; i < len; i++)
    {
        if ((32 <= (*(temporary + i))) && ((*(temporary + i)) <= 126))
        {
            printf("%c", (*temporary+i));
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool print2(const void *buf, int len) {
     const char *p;
    p = buf;
    while (len > 0)
    {
        printf("%c", *p);
        p++;
        len--;
    }
    return true;
}
