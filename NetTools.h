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
#include <ifaddrs.h>

#define CLEARBUFFER while ( (c = getchar()) != '\n' && c != EOF);

#define MAXADDRLEN 40
#define MAXSERVLEN 35

#define FULLSCAN 1
#define TGTSCAN 2
#define RNGSCAN 3

int tcpPortScan(int flag);

int getLocalAddress(void);