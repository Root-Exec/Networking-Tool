#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <string.h>

#define CLEARBUFFER while ( (c = getchar()) != '\n' && c != EOF);

#define MAXADDRLEN 30
#define MAXSERVLEN 15

#define FULLSCAN 1
#define TGTSCAN 2
#define RNGSCAN 3

void fullPortScan(void) {

    struct addrinfo filter, *results, *record;
    char target[MAXADDRLEN];
    char service[MAXSERVLEN];
    char ipaddr[MAXADDRLEN];
    
    int sock, status, connection, port, connectTimeOut;

    struct timeval timeout;
    fd_set network_set;

    printf("Enter target name or IP address: ");
    fgets(target, MAXADDRLEN, stdin);
    for (int i = 0; i < MAXADDRLEN; i++) {
        if (*(target + i) == '\n') *(target + i) = '\0';
    }

    memset(&filter, 0, sizeof(struct addrinfo));
    filter.ai_socktype = SOCK_STREAM;
    filter.ai_family = AF_INET;
    filter.ai_flags = AI_PASSIVE;

    port = 443;

    while (port < 500) {

        memset(service, 0, MAXSERVLEN);
        snprintf(service, MAXSERVLEN, "%d", port);

        status = getaddrinfo(target, service, &filter, &results);
        if (status != 0) {
            perror("Address resolution failure");
        }
        
        for (record = results; record != NULL; record = record->ai_next) {

            memset(ipaddr, 0, sizeof(char) * MAXADDRLEN);
            inet_ntop(record->ai_family, record->ai_addr->sa_data, ipaddr, MAXADDRLEN);  
            printf("%s:%s.......", ipaddr, service);

            sock = socket(record->ai_family, record->ai_socktype, record->ai_protocol);
            fcntl(sock, F_SETFL, O_NONBLOCK);
            connection = connect(sock, record->ai_addr, record->ai_addrlen);
            
            timeout.tv_sec = 3;
            timeout.tv_usec = 0;
            FD_ZERO(&network_set);
            FD_SET(sock, &network_set);
            connectTimeOut = select(sock + 1, NULL, &network_set, NULL, &timeout);

            if (connectTimeOut == 0) {
                printf("Closed Port\n");
                close(sock);
                continue;
            }

            printf("Open Port\n");
            close(sock);
        
        }
        memset(results, 0, sizeof(struct addrinfo));
        close(sock);
        port++;
        
    }
    freeaddrinfo(results);
}


void portScan(int flag) {

    struct addrinfo filter, *results, *record;
    char target[MAXADDRLEN];
    char service[MAXSERVLEN];
    char ipaddr[MAXADDRLEN];
    
    int sock, status, connection, port = 0, connectTimeOut, end = 0, c, fileStatus;

    struct timeval timeout;
    fd_set network_set;

    printf("Enter target name or IP address: ");
    fgets(target, MAXADDRLEN, stdin);
    target[strlen(target) - 1] = '\0';

    memset(&filter, 0, sizeof(struct addrinfo));
    filter.ai_socktype = SOCK_STREAM;
    filter.ai_family = AF_INET;
    filter.ai_flags = AI_PASSIVE;

    memset(service, 0, MAXSERVLEN);

    if (flag == TGTSCAN) {
        printf("Enter service or port: ");
        fgets(service, MAXSERVLEN, stdin);
        service[strlen(service) - 1] = '\0';
        start = port = end = 0;
    } else if (flag == FULLSCAN) {
        port = 0;
        end = 1024;
    } else if (flag == RNGSCAN) {
        printf("Enter starting port: ");
        scanf("%d", &port);
        CLEARBUFFER
        printf("Enter ending port: ");
        scanf("%d", &end);
        CLEARBUFFER
    }

    while (port <= end) {

        if (flag != TGTSCAN) {
            snprintf(service, MAXSERVLEN, "%d", port);
        }

        status = getaddrinfo(target, service, &filter, &results);
        if (status != 0) {
            perror("Address resolution failure");
            continue;
        }

        for (record = results; record != NULL; record = record->ai_next) {

            memset(ipaddr, 0, sizeof(char) * MAXADDRLEN);
            inet_ntop(record->ai_family, record->ai_addr->sa_data, ipaddr, MAXADDRLEN);  
            printf("%s:%s.......", ipaddr, service);

            sock = socket(record->ai_family, record->ai_socktype, record->ai_protocol);
            if (sock == -1) {
                printf("Failed to set up socket\n");
                continue;
            }

            fileStatus = fcntl(sock, F_SETFL, O_NONBLOCK);
            if (fileStatus == -1) {
                perror("Connection not set to non-blocking mode");
                printf("Continuing to next port...\n");
                fclose(socket);
                continue;
            }

            connection = connect(sock, record->ai_addr, record->ai_addrlen
            if (connection < 0) {
                perror("Socket Connection failure");
                close(sock);
                continue;
            }
            
            timeout.tv_sec = 2;
            timeout.tv_usec = 0;
            FD_ZERO(&network_set);
            FD_SET(sock, &network_set);

            connectTimeOut = select(sock + 1, NULL, &network_set, NULL, &timeout);

            if (connectTimeOut == 0) {
                printf("Closed Port\n");
                close(sock);
                continue;
            }

            printf("Open Port\n");
            close(sock);
        
        }
        memset(results, 0, sizeof(struct addrinfo));
        close(sock);
        port++;
    }
    freeaddrinfo(results);
}