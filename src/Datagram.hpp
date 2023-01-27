#ifndef __DATAGRAM_HPP_ 
#define __DATAGRAM_HPP_

#define DATAGRAM_LENGTH 1024

typedef struct Datagram{
    char data[DATAGRAM_LENGTH];
}Datagram;

#endif