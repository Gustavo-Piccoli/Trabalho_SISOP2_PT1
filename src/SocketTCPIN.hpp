#ifndef __SOCKETTCPIN_HPP_
#define __SOCKETTCPIN_HPP_

#include <sys/socket.h>
#include <netinet/in.h>

class SocketTCPIN{
private:
    int descriptor;
public:
    SockeSocketTCPINt();
    ~SocketTCPIN();
};

SocketTCPIN::SocketTCPIN(int port ){
    // Create Socket
    this->descriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Bind

}

SocketTCPIN::~SocketTCPIN(){
}



#endif