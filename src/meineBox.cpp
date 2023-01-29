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
#include "clientStateInformationStruct.hpp"
#include "userTerminalFunctions.hpp"

using namespace std;

// Prototypes
void *userTerminalThread(void*);
void *syncronizationModuleThread(void*);


int main(){
  cout << "======== MeineBox: A litle piece of cloud in a Box ========" << endl;
  cout << "\t\t\t==== v"<< MEINEBOX_VERSION <<" ====" << endl;
  cout << "  >> Inicializing ..." << endl;

  // Create Data and Sync Communication Sockets
  int info_data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);
  int sync_data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);

  if(info_data_communication_socket == -1)
    pError("\a  ##Error on INFO data communication socket creation!");
  else
    cout << "  >> INFO Data Socket created successfully ..." << endl;

  if(sync_data_communication_socket == -1)
    pError("\a  ##Error on SYNC data communication socket creation!");
  else
    cout << "  >> SYNC Data Socket created successfully ..." << endl;

  // Connect Info and Sync Data Communication Sockets
  struct sockaddr_in server_sentinel_socket_addr; 
  server_sentinel_socket_addr.sin_family = AF_INET;
  server_sentinel_socket_addr.sin_port = SENTINEL_SOCKET_PORT;
  server_sentinel_socket_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(server_sentinel_socket_addr.sin_zero), 8);
  
  if(connect(info_data_communication_socket, (const sockaddr *)&server_sentinel_socket_addr, sizeof(server_sentinel_socket_addr) ) == -1)
    pError("\a  ##Failure: No server return for attemp of INFO Data Communication Socket conection!");   
  if(connect(sync_data_communication_socket, (const sockaddr *)&server_sentinel_socket_addr, sizeof(server_sentinel_socket_addr) ) == -1)
    pError("\a  ##Failure: No server return for attemp of SYNC Data Communication Socket conection!");   
  
  cout << "  >> Connection to server established successfully ..." << endl;
  cout << "  >> Thanks for use MeineBox. Type 'login' if you have an account or 'register' to sign in" << endl;

  // Aplication User Terminal
  pthread_t user_terminal_thread;
  ClientStateInformation clientStateInformation;
  clientStateInformation.info_data_communication_socket = info_data_communication_socket;
  clientStateInformation.is_syncronization_active = false;
  clientStateInformation.is_user_logged = false;
  strcpy(clientStateInformation.userMeineBox.login, "No one logged in");
  strcpy(clientStateInformation.userMeineBox.passwd, "No one logged in");

  pthread_create(&user_terminal_thread, NULL, userTerminalThread, (void*)&clientStateInformation);

  // Syncronization Module
  pthread_t syncronization_module_thread;
  clientStateInformation.sync_data_communication_socket = sync_data_communication_socket;
  pthread_create(&syncronization_module_thread, NULL, syncronizationModuleThread, (void*)&clientStateInformation);


    // Write in socket
    // write(info_data_communication_socket, &data_buffer, strlen(data_buffer));
    
    // Read from socket 


  // Close
  pthread_join(user_terminal_thread, NULL);
  close(info_data_communication_socket);
  return 0;
}

void *userTerminalThread( void *clientStateInformation_arg ){
  ClientStateInformation *clientStateInformation = (ClientStateInformation*)clientStateInformation_arg;
  string command("");
  
  while(true){
    cout << "$ ";
    cin >> command;

    if(command == "help"){
      userTerminal_help();
    }else if(command == "login"){
      userTerminal_login(clientStateInformation);
    }else if(command == "start"){
      userTerminal_start(clientStateInformation);
    }else if(command == "status"){
      userTerminal_status(clientStateInformation);
    }else if(command == "stop"){
      userTerminal_stop(clientStateInformation);
    }else if(command == "register"){
      userTerminal_register(clientStateInformation);
    }else if(command == "quit"){
      clientStateInformation->is_syncronization_active = false;
      return NULL;
    }else{
      cout << endl << "  Command '" << command << "' not found" << endl;
    }

  }

}


void *syncronizationModuleThread( void *clientStateInformation_arg ){
  ClientStateInformation *clientStateInformation = (ClientStateInformation*)clientStateInformation_arg;
  
  while(true){

  }

}