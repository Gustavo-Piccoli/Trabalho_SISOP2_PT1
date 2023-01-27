#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "errorHandling.hpp"
#include "global_definitions.hpp"

#define SENTINEL_SOCKET_PORT 7777
#define SENTINEL_SOCKET_QUEUE_CAPACITY 10

#define MEINEBOX_VERSION 0.1

using namespace std;

// Prototypes
void *userConsoleThread(void*);


int main(){
  cout << "  >> Inicializing ..." << endl;

  // Create Data Communication Socket
  int data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(data_communication_socket == -1)
    pError("\a  ##Error on data communication socket creation!");
  else
    cout << "  >> Socket created successfully" << endl;

  // Connect Data Communication Socket
  struct sockaddr_in data_communication_socket_addr;
  data_communication_socket_addr.sin_family = AF_INET;
  data_communication_socket_addr.sin_port = SENTINEL_SOCKET_PORT;
  data_communication_socket_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(data_communication_socket_addr.sin_zero), 8);
  
  if(connect(data_communication_socket, (const sockaddr *)&data_communication_socket_addr, sizeof(data_communication_socket_addr) ) == -1)
    pError("\a  ##Failure: No server return on Data Communication Socket conection!");   
  else
    cout << "  >> Connection to server established successfully" << endl;

  // Aplication User Terminal
  bool is_syncronization_active = false;
  pthread_t user_console_thread;

  pthread_create(&user_console_thread, NULL, userConsoleThread, (void*)&is_syncronization_active);

    // Write in socket
    // char data_buffer[1000] = "Hello MeineBox Server! I hope so you are good. Regards from MeineBox client1";
    // write(data_communication_socket, &data_buffer, strlen(data_buffer));
    
    // Read from socket 


  // Close
  pthread_join(user_console_thread, NULL);
  close(data_communication_socket);
  return 0;
}

void *userConsoleThread( void *is_syncronization_active_arg  ){
  bool *is_syncronization_active = (bool*)is_syncronization_active_arg;
  string command("");
  
  cout << "======== MeineBox: A litle piece of cloud in a Box ========" << endl;
  cout << "\t\t\t==== v"<< MEINEBOX_VERSION <<" ====" << endl;

  while(true){
    cout << "$ ";
    cin >> command;

    if(command == "start"){
      if( *is_syncronization_active )
        cout << "  Service is already active!" << endl;
      else
        *is_syncronization_active = true;
    }else if(command == "stop"){
      if( !(*is_syncronization_active) )
        cout << "  Service is already paused!" << endl;
      else
        *is_syncronization_active = false;
    }else if(command == "status"){
      if(*is_syncronization_active){
        cout << "  Active Service!" << endl;
      }else
        cout << "  Inactive Service!" << endl;
    }else if(command == "quit"){
      *is_syncronization_active = false;
      return NULL;
    }else{
      cout << endl << "  Command '" << command << "' not found" << endl;
    }

  }

}