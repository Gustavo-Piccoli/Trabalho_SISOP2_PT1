#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "errorHandling.hpp"

#define SENTINEL_SOCKET_PORT 7777
#define SENTINEL_SOCKET_QUEUE_CAPACITY 10

using namespace std;

int main(){
    // Create Data Communication Socket
    int data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(data_communication_socket == -1)
      pError("\aError on data communication socket creation!");
    else
      cout << ">> Socket created successfully" << endl;

    // Connect Data Communication Socket
    struct sockaddr_in data_communication_socket_addr;
    data_communication_socket_addr.sin_family = AF_INET;
    data_communication_socket_addr.sin_port = SENTINEL_SOCKET_PORT;
    data_communication_socket_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(data_communication_socket_addr.sin_zero), 8);
    
    if(connect(data_communication_socket, (const sockaddr *)&data_communication_socket_addr, sizeof(data_communication_socket_addr) ) == -1)
      pError("\aError on Data Communication Socket conection!");   
    else
      cout << ">> Connection to server established successfully" << endl;
    
    // Write in socket
    char data_buffer[1000] = "Hello MeineBox Server! I hope so you are good. Regards from MeineBox client1";
    write(data_communication_socket, &data_buffer, strlen(data_buffer));
    
    // Read from socket 


    // Close
    close(data_communication_socket);
}

