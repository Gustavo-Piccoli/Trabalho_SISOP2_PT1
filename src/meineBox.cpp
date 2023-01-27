#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
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


using namespace std;

int main(){
    // Creation
    int socket_descriptor =  socket(AF_UNIX, SOCK_DGRAM, 0);
    if( socket_descriptor == -1)
      perror("Error socket creation");

    // Send
    Datagram datagram_buffer;
    struct sockaddr_un server_address;
    
    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, SERVER_PATH);

    strcpy( datagram_buffer.data, "Hello server. The learning operation today was susccessfully. See you later. Take care!" );

    int bytes_sended;
    bytes_sended = sendto(socket_descriptor, datagram_buffer.data, DATAGRAM_LENGTH, 0, (const struct sockaddr*) &server_address, (socklen_t) sizeof(server_address));
    
    cout << "Mensagem Enviada!" << endl;
    cout << "\t Length: " << bytes_sended << " Bytes" << endl;
    cout << "\t Destinatario: " << server_address.sun_path << endl;
    cout << "\t Message: " << datagram_buffer.data << endl;
    cout << "Encerrando ..." << endl;

    //Recieve

    // Close


}