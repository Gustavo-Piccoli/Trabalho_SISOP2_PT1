#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "errorHandling.hpp"
#include "userMeineBox.hpp"
#include "serverCommunicationManager.hpp"
#include "global_definitions.hpp"


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
    
    // Server's New Device Sentinel Service Thread Initialization
    pthread_t newDeviceSentinelServiceThread;

    if( pthread_create( &newDeviceSentinelServiceThread, NULL, serverNewDeviceSentinelService, (void*)&servers_sentinel_socket) != 0 )
        pError("Error on initizalization of New Device Sentinel Service Thread");    
    else
        cout << ">> 'New Device Sentinel' Service initialized successufully" << endl;




//     // Read from Data Socket
//     char data_buffer[1000] = "";
//     size_t bytes_read;
//     bytes_read = read(info_data_communication_socket, &data_buffer, 1000);

//     if( bytes_read < 0 )
//         pError("\a### Error on reding from data communication socket!");
//     else{
//         cout << ">> Mesage recieved: " << endl;    
//         cout << "\t> Length: " << bytes_read << " Bytes" << endl;    
//         cout << "\t> Content: " << data_buffer << endl;    
//     }

    // Write on Data Socket
    // Close
while(1);
    close(servers_sentinel_socket);
//     close(info_info_data_communication_socket);
    return 0;
}
