#include <iostream>
#include <string>
#include <cstring>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h> 

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

  // Create INFO Data Communication Sockets
  int info_data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);

  if(info_data_communication_socket == -1)
    pError("\a  ##Error on INFO data communication socket creation!");
  else
    cout << "  >> INFO Data Socket created successfully ..." << endl;

  // Localize Server Host
  struct hostent *meineBox_server_host = gethostbyname("localhost");
  if(meineBox_server_host == NULL)
    pError("\a  ##Failure: Can't localize server address!");   
  else
    cout << "  >> Server address localized successfully!" << endl;

  // Connect Info Data Communication Sockets
  struct sockaddr_in server_sentinel_socket_addr; 
  server_sentinel_socket_addr.sin_family = AF_INET;
  server_sentinel_socket_addr.sin_port = htons(SENTINEL_SOCKET_PORT);
  server_sentinel_socket_addr.sin_addr = *((struct in_addr*)meineBox_server_host->h_addr_list[0]);
  bzero(&(server_sentinel_socket_addr.sin_zero), 8);
  
  if(connect(info_data_communication_socket, (struct sockaddr *)&server_sentinel_socket_addr, sizeof(server_sentinel_socket_addr) ) < 0 )
    pError("\a  ##Failure: No server return for attemp of INFO Data Communication Socket conection!");   

  // TEMP socket to listen SYNC Data socket
  int temp_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in  temp_socket_addr;
  temp_socket_addr.sin_family = AF_INET;
  temp_socket_addr.sin_port = htons(CLIENT_RECEIVE_CONNECTION_PORT);
  temp_socket_addr.sin_addr.s_addr = INADDR_ANY;
  bzero(&(temp_socket_addr.sin_zero), 8);

  if(bind(temp_socket, (struct sockaddr *)&temp_socket_addr, sizeof(temp_socket_addr)) == -1)
      pError("\a### Error on TEMP socket binding!");
  else
      cout << "  >> TEMP Socket binded to port "<< CLIENT_RECEIVE_CONNECTION_PORT << " successfully" << endl;
  
  // Listen in TEMP socket to create SYNC Data socket
  if(listen(temp_socket,SYNC_SOCKET_QUEUE_CAPACITY) == -1)
    pError("\a  ##Failure: Can't listen on TEMP socket to recieve SYNC socket!");   
  else
    cout << "  >> TEMP socket listen: Awaiting for server request for SYNC socket ..." << endl;

  // Accept SYNC Data socket
  sockaddr_in server_sync_socket_address;
  socklen_t server_sync_socket_address_len = sizeof(struct sockaddr_storage);
  int sync_data_communication_socket = accept(temp_socket, (struct sockaddr *)&server_sync_socket_address, &server_sync_socket_address_len);
  close(temp_socket);

  if(sync_data_communication_socket == -1)
    pError("\a  ##Error on SYNC data communication socket creation(acceptance)!");
  else
    cout << "  >> SYNC Data Socket created(accepted) successfully ..." << endl;


  cout << "  >> Connection to server established successfully ..." << endl;
  cout << "  >> Thanks for use MeineBox. Type 'login' if you have an account or 'register' to sign in" << endl;

  // Aplication User Terminal
  pthread_t user_terminal_thread;
  ClientStateInformation clientStateInformation;
  clientStateInformation.info_data_communication_socket = info_data_communication_socket;
  clientStateInformation.is_syncronization_active = false;
  clientStateInformation.is_user_logged = false;
  clientStateInformation.is_connectded = true;
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
  
  while(clientStateInformation->is_connectded){
    // Froze if client stop service
    while(!clientStateInformation->is_syncronization_active){};

    // Send Sync List
    //write(sync list in sync_data_socket);

    // Read order from server
    //read(order from sync_data_socket);

    if(/*Server orders to read*/1){
        //read(datagram from sync_data_socket);
    }else if(/*Server orders to write*/1){
        // write(datagram to sync_data_socket);
    }
  }

  return NULL;
}