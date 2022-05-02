#include "NetTools.h"

int tcpPortScan(int flag) {

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
        port = end = 0;
    } else if (flag == FULLSCAN) {
        port = 0;
        end = 1024;
    } else if (flag == RNGSCAN) {
        printf("Enter starting port: ");
        scanf("%d", &port);
        if (port < 0) {
            printf("Invalid port number, exiting.");
            exit(EXIT_FAILURE);
        }
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
            port++;
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
                close(sock);
                continue;
            }

            connection = connect(sock, record->ai_addr, record->ai_addrlen);
            if (connection < 0) {
                perror("Socket Connection failure");
                close(sock);
                continue;
            }
            
            timeout.tv_sec = 1;
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
    exit(EXIT_SUCCESS);
}


int getLocalAddress(void) {
    struct ifaddrs *localAddressList, *localAddress;
    char hostAddress[NI_MAXHOST];
    char service[NI_MAXSERV];
    int family, status;

    status = getifaddrs(&localAddressList);
    if (status == - 1) {
        perror("Failed to retrieve local address structure");
        exit(EXIT_FAILURE);
    }

    for (localAddress = localAddressList; localAddress != NULL; localAddress = localAddress->ifa_next) {
        if (localAddress->ifa_addr == NULL) continue;
        family = localAddress->ifa_addr->sa_family;

        if (family == AF_INET || family == AF_INET6) {
            status = getnameinfo(localAddress->ifa_addr, 
            (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof( struct sockaddr_in6),
            hostAddress, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST);

            if (status != 0) {
                printf("getnameinfo() failed to resolve: %s\n", gai_strerror(status));
                exit(EXIT_FAILURE);
            }
            
            if (family == AF_INET) printf("%s\tAF_INET Address: <%s>\n", service, hostAddress);
            else printf("%s\tAF_INET6 Address: <%s>\n", service, hostAddress);
            
        }

    }


    return 0; 
}