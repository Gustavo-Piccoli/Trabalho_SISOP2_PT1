#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

#include "Datagram.hpp"

/** UDP - SERVER
     Sumary:    
        -> Creation
        -> Bind
        -> Recieve
        -> Send
        -> Close
*/

#define SERVER_PORT 7777

using namespace std;

int main(){
    // Creation
    int socket_descriptor =  socket(AF_INET, SOCK_DGRAM, 0);
    if( socket_descriptor == -1)
      perror("Error socket creation");

    // Bind
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_addr.s_addr = INADDR_ANY; 
    bzero(&(server_address.sin_zero), 8);   
    
    if( bind(socket_descriptor, (struct sockaddr *)&server_address, sizeof(struct sockaddr) ) < 0 )
      perror("Error on socket binding");
   
    // Recieve
    Datagram datagram_buffer;
    struct sockaddr *client_address = new struct sockaddr;
    socklen_t *client_address_length = new socklen_t;
    int bytes_received;
    
    cout << "Servidor na escuta. Câmbio ..." << endl;
    bytes_received = recvfrom(socket_descriptor, datagram_buffer.data , DATAGRAM_LENGTH, 0, client_address, client_address_length);
    cout << "\a++ Message Received: " << endl;
    cout << "\t Client: " << client_address->sa_data << endl;
    cout << "\t Address Legth: " << client_address_length << endl;
    cout << "\t Bytes Received: " << bytes_received  << endl;
    cout << "\t\t Contents: " <<  datagram_buffer.data << endl;

    // Send

    // Close
    delete client_address;
    delete client_address_length;




}