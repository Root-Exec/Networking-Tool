#include "PortScanner.h"

//RNGSCAN flag allows user to specify a range of ports to search
//TGTSCAN flag allows user to knowingly target a specific port to attempt a connection
//FULLSCN flag forces the program to search through all ports listed 0 through 1024

int main (void) {

    portScan(RNGSCAN);


    return 0;
}