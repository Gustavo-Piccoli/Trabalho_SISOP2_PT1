#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "errorHandling.hpp"
#include "global_definitions.hpp"
#include "userMeineBox.hpp"
#include "userTerminalThreadInfo.hpp"
#include "userTerminalFunctions.hpp"

using namespace std;

// Prototypes
void *userTerminalThread(void*);


int main(){
  cout << "======== MeineBox: A litle piece of cloud in a Box ========" << endl;
  cout << "\t\t\t==== v"<< MEINEBOX_VERSION <<" ====" << endl;
  cout << "  >> Inicializing ..." << endl;

  // Create Data Communication Socket
  int data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(data_communication_socket == -1)
    pError("\a  ##Error on data communication socket creation!");
  else
    cout << "  >> Socket created successfully ..." << endl;

  // Connect Data Communication Socket
  struct sockaddr_in data_communication_socket_addr;
  data_communication_socket_addr.sin_family = AF_INET;
  data_communication_socket_addr.sin_port = SENTINEL_SOCKET_PORT;
  data_communication_socket_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(data_communication_socket_addr.sin_zero), 8);
  
  if(connect(data_communication_socket, (const sockaddr *)&data_communication_socket_addr, sizeof(data_communication_socket_addr) ) == -1)
    pError("\a  ##Failure: No server return on Data Communication Socket conection!");   
  else
    cout << "  >> Connection to server established successfully ..." << endl;
    cout << "  >> Thanks for use MeineBox. Type 'login' if have an account or 'register' to sign in" << endl;

  // Aplication User Terminal
  pthread_t user_terminal_thread;
  UserTerminalThreadInfo userTerminalThreadInfo;
  userTerminalThreadInfo.data_communication_socket = data_communication_socket;

  pthread_create(&user_terminal_thread, NULL, userTerminalThread, (void*)&userTerminalThreadInfo);

    // Write in socket
    // write(data_communication_socket, &data_buffer, strlen(data_buffer));
    
    // Read from socket 


  // Close
  pthread_join(user_terminal_thread, NULL);
  close(data_communication_socket);
  return 0;
}

void *userTerminalThread( void *userTerminalThreadInfo ){
  int data_communication_socket = ((UserTerminalThreadInfo*)userTerminalThreadInfo)->data_communication_socket;
  bool *is_syncronization_active = &(((UserTerminalThreadInfo*)userTerminalThreadInfo)->is_syncronization_active);
  string command("");
  
  while(true){
    cout << "$ ";
    cin >> command;

    if(command == "help"){
      userTerminal_help();
    }else if(command == "login"){
      userTerminal_login(data_communication_socket);
    }else if(command == "start"){
      userTerminal_start(is_syncronization_active);
    }else if(command == "status"){
      userTerminal_status(is_syncronization_active);
    }else if(command == "stop"){
      userTerminal_stop(is_syncronization_active);
    }else if(command == "register"){
      userTerminal_register(data_communication_socket);
    }else if(command == "quit"){
      *is_syncronization_active = false;
      return NULL;
    }else{
      cout << endl << "  Command '" << command << "' not found" << endl;
    }

  }

}