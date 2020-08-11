#define _POSIX_C_SOURCE 200809L
#define _BSD_SOURCE
#define _DEFAULT_SOURCE
#include <sys/types.h> // *_t, u_int16_t
#include <netinet/in.h>
#include <arpa/nameser.h> //  NS_* i ns_*
#include <resolv.h>       //  nagłówek, res_search
#include <netdb.h>        // h_errno i herror

#include <stdio.h>      // printf
#include <stdlib.h>     // malloc
#include <string.h>     // memset, memcpy
#include <netinet/in.h> // struct in_addr
#include <arpa/inet.h>  // inet_ntoa
#include <unistd.h>
#include <iostream>
#define ARRAY_LENGTH 32
#define MAX_SIZE 1024
#define PORT 25
void client_fun(struct sockaddr_in client_address);
static int query(const char *domain_name, int type,
                 struct in_addr *results);
static int process_mx_record(ns_msg *handle, ns_rr *record,
                             struct in_addr *results);
static int process_a_record(ns_msg *handle, ns_rr *record,
                            struct in_addr *results);
static void add(struct in_addr *results, struct in_addr address);
static char *return_tab(char *buf);
struct in_addr *find_mail_exchanges(const char *domain_name);

struct in_addr *find_mail_exchanges(const char *domain_name)
{
#ifdef TRACE
    printf("finding MX-es for %s\n", domain_name);
#endif

    struct in_addr *results = (struct in_addr *)
        malloc(ARRAY_LENGTH * sizeof(struct in_addr));
    if (results == NULL)
    {
        fputs("malloc: out of memory\n", stderr);
        return NULL;
    }
    for (int i = 0; i < ARRAY_LENGTH; ++i)
    {
        results[i].s_addr = htonl(INADDR_ANY);
    }

    int status;
    status = query(domain_name, ns_t_mx, results);
    if (status == 0)
    {
#ifdef TRACE
        printf("no MX records found, trying A records\n");
#endif
        status = query(domain_name, ns_t_a, results);
    }
    if (status == -1 || results[0].s_addr == htonl(INADDR_ANY))
    {
        fputs("find_mail_exchanges: none found\n", stderr);
        free(results);
        return NULL;
    }

#ifdef TRACE
    printf("search done, found some IPv4 adresses\n");
#endif

    return results;
}
static int query(const char *domain_name, int type, struct in_addr *results)
{
    int status;

#ifdef TRACE
    printf("querying DNS for type %i records for %s\n", type, domain_name);
#endif

    unsigned char reply[NS_PACKETSZ];
    int reply_len;

    reply_len = res_search(domain_name, ns_c_in, type, reply, sizeof(reply));
    if (reply_len == -1)
    {
        if (h_errno == NO_DATA)
        {
            return 0;
        }
        else
        {
            herror("res_search");
            return -1;
        }
    }

    ns_msg handle;

    status = ns_initparse(reply, reply_len, &handle);
    if (status == -1)
    {
        fputs("ns_initparse: truncated or malformed packet\n", stderr);
        return -1;
    }

    u_int16_t answers_count = ns_msg_count(handle, ns_s_an);

    ns_rr record;

    for (int i = 0; i < answers_count; ++i)
    {
        status = ns_parserr(&handle, ns_s_an, i, &record);
        if (status == -1)
        {
            fputs("ns_parserr: truncated or malformed record\n", stderr);
            return -1;
        }

#ifdef TRACE
        char buf[1024];
        ns_sprintrr(&handle, &record, NULL, NULL, buf, sizeof(buf));
        printf("%s\n", buf);
#endif

        if (type == ns_t_mx)
        {
            status = process_mx_record(&handle, &record, results);
        }
        else if (type == ns_t_a)
        {
            status = process_a_record(&handle, &record, results);
        }
        else
        {
            return -1;
        }
        if (status == -1)
        {
            return -1;
        }
    }

    return answers_count;
}

static int process_mx_record(ns_msg *handle, ns_rr *record,
                             struct in_addr *results)
{
    if (ns_rr_type(*record) != ns_t_mx)
    {
        fputs("unexpected resource record type\n", stderr);
        return -1;
    }

    const u_char *msg_begin = ns_msg_base(*handle);
    const u_char *msg_end = msg_begin + ns_msg_size(*handle);
    const u_char *data = ns_rr_rdata(*record);
    u_int16_t data_len = ns_rr_rdlen(*record);

    if (data_len < 3)
    {
        fputs("malformed (too short) resource record\n", stderr);
        return -1;
    }

    u_int prio = ns_get16(data);

    char mx_name[MAXDNAME];
    int status = ns_name_uncompress(msg_begin, msg_end, data + 2,
                                    mx_name, sizeof(mx_name));

    if (status == -1 || data_len != 2 + status)
    {
        fputs("malformed resource record\n", stderr);
        return -1;
    }

#ifdef TRACE
    printf("resolving MX = %s (priority %u)\n", mx_name, prio);
#endif

    status = query(mx_name, ns_t_a, results);
    if (status == 0)
    {
        fputs("failed to resolve MX name to IPv4 address\n", stderr);
    }

    return status;
}

static int process_a_record(ns_msg *handle, ns_rr *record,
                            struct in_addr *results)
{
    if (ns_rr_type(*record) != ns_t_a)
    {
        fputs("unexpected resource record type\n", stderr);
        return -1;
    }

    const u_char *data = ns_rr_rdata(*record);
    u_int16_t data_len = ns_rr_rdlen(*record);

    if (data_len != 4)
    {
        fputs("malformed resource record\n", stderr);
        return -1;
    }

    struct in_addr address;
    memset(&address, 0, sizeof(address));
    memcpy(&address.s_addr, data, 4);

#ifdef TRACE
    printf("adding A = %s\n", inet_ntoa(address));
#endif

    add(results, address);

    return 0;
}
static void add(struct in_addr *results, struct in_addr address)
{
    for (int i = 0; i < ARRAY_LENGTH - 1; ++i)
    {
        if (results[i].s_addr == htonl(INADDR_ANY))
        {
            results[i] = address;
            return;
        }
    }
    fputs("find_mail_exchanges: too many results\n", stderr);
}

void client_fun(struct sockaddr_in client_address)
{
    int client;
    char buff[MAX_SIZE];
    int read_size;
    printf("Greetings from \t");
    if ((client = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot creat socket");
        exit(1);
    }
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
    for (int i = 0; i < read_size; i++)
    {
        if (((int)(buff[i]) < 127) && ((int)buff[i] > 32))
        {
            printf("%c", buff[i]);
        }
    }
    printf("\n");
    if (close(client) == -1)
    {
        perror("Cannot close the client");
        exit(4);
    }
}

char *return_tab(char *buf)
{
    int i = 0;
    int i_d = 0;
    while (*(buf + i) != 0)
    {
        if (('@' == *(buf + i)))
        {
            while (*(buf + i) != 0)
            {
                i++;
                *(buf + i_d) = *(buf + i);
                i_d++;

            }
            return buf;
        }
        i++;
    }
    return buf;
}



int main(int argc, char *argv[])
{
    char *domain;
    struct sockaddr_in client_address;

    for (int i = 1; i < argc; i++)
    {
        domain = return_tab(argv[i]);
        struct in_addr *adresses = find_mail_exchanges(domain);
        if (adresses == NULL)
        {
            return 1;
        }
        printf(" Greatings from domain %d \n ", i);
        printf("------------------------------------\n");

        char buf[INET_ADDRSTRLEN];
        for (int i = 0; adresses[i].s_addr != htonl(INADDR_ANY); ++i)
        {
            if (inet_ntop(AF_INET, adresses + i, buf, sizeof(buf)) == NULL)
            {
                std::perror("inet_ntop");
                return 1;
            }
            memset(&client_address, 0, sizeof(struct sockaddr));
            client_address.sin_family = AF_INET;
            client_address.sin_addr.s_addr = inet_addr(buf);
            client_address.sin_port = htons(PORT);
            client_fun(client_address);
        }
        printf("------------------------------------\n");

        std::free(adresses);
    }
}


