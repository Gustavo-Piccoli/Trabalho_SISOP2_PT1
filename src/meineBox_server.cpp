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
    // Create Sentinel Socket
    int servers_sentinel_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(servers_sentinel_socket == -1)
        pError("\a### Error on sentinel socket creation!");
    else
        cout << ">> Server Sentinel Socket created successfully" << endl;

    // Bind Sentinel Socket
    struct sockaddr_in sentinel_server_socket_addr;
    sentinel_server_socket_addr.sin_family = AF_INET;
    sentinel_server_socket_addr.sin_port = SENTINEL_SOCKET_PORT;
    sentinel_server_socket_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(sentinel_server_socket_addr.sin_zero), 8);

    if( bind(servers_sentinel_socket, (const sockaddr *)&sentinel_server_socket_addr, sizeof(sentinel_server_socket_addr)) < 0 )
        pError("\a### Error on sentinel socket binding!");
    else
        cout << ">> Server Sentinel Socket binded to port "<< SENTINEL_SOCKET_PORT << " successfully" << endl;
    
    // Listen
    if( listen(servers_sentinel_socket, SENTINEL_SOCKET_QUEUE_CAPACITY) == -1 ){
        cout << "\a### Failure in listen for connection!" << endl;
     }else{
        cout << ">> Awaiting for connection attempt ..." << endl;
     }

    // Accept
    struct sockaddr_in data_communication_socket_addr;
    socklen_t data_communication_socket_addr_length;
    int data_communication_socket = accept(servers_sentinel_socket, (struct sockaddr *)&data_communication_socket_addr, &data_communication_socket_addr_length);
    if(data_communication_socket == -1){
        cout << "\a### Failure in accept connection!" << endl;
    }else{
        cout << ">> Connection established: " << endl;
        cout << "\t> Port: " << data_communication_socket_addr.sin_port << endl;
        cout << "\t> Address: " << data_communication_socket_addr.sin_addr.s_addr << endl;
        cout << "\t> Address Family: " << data_communication_socket_addr.sin_family << endl;
        cout << "\t> Address Length: " << data_communication_socket_addr_length << endl;
    }

    // Read from Data Socket
    char data_buffer[1000] = "";
    size_t bytes_read;
    bytes_read = read(data_communication_socket, &data_buffer, 1000);

    if( bytes_read < 0 )
        pError("\a### Error on reding from data communication socket!");
    else{
        cout << ">> Mesage recieved: " << endl;    
        cout << "\t> Length: " << bytes_read << " Bytes" << endl;    
        cout << "\t> Content: " << data_buffer << endl;    
    }

    // Write on Data Socket
    // Close
    close(servers_sentinel_socket);
    close(data_communication_socket);
}

