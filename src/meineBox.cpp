#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#include "Datagram.hpp"

/** UDP - Client
     Sumary:    
        -> Creation
        -> Send
        -> Recieve
        -> Close
*/
#define HOST_NAME "localhost"
#define SERVER_PORT 7777

using namespace std;

int main(){
    // Creation
    int socket_descriptor =  socket(AF_INET, SOCK_DGRAM, 0);
    if( socket_descriptor == -1)
      perror("Error socket creation");

    // Send
    Datagram datagram_buffer;
    struct hostent *server = gethostbyname(HOST_NAME);
    struct sockaddr_in server_address;
    
    server_address.sin_addr = *((struct in_addr*)server->h_addr_list[0]);
    server_address.sin_port = htons(SERVER_PORT);
    server_address.sin_family = AF_INET;
    bzero(&(server_address.sin_zero), 8); 

    strcpy( datagram_buffer.data, "Hello server. The learning operation today was susccessfully. See you later. Take care!" );

    int bytes_sended;
    bytes_sended = sendto(socket_descriptor, datagram_buffer.data, DATAGRAM_LENGTH, 0, (const struct sockaddr*) &server_address, (socklen_t) sizeof(server_address));
    
    cout << "Mensagem Enviada!" << endl;
    cout << "\t Length: " << bytes_sended << " Bytes" << endl;
    cout << "\t Destinatario: " << server_address.sin_addr.s_addr << endl;
    cout << "\t Message: " << datagram_buffer.data << endl;
    cout << "Encerrando ..." << endl;

    //Recieve

    // Close


}