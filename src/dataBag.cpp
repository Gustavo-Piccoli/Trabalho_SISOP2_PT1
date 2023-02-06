#include <iostream>
#include <string>
#include <cstring>
#include <fstream> 


#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <netdb.h> 

#include "errorHandling.hpp"
#include "global_definitions.hpp"
#include "userDataBag.hpp"
#include "clientStateInformationStruct.hpp"
#include "userTerminalFunctions.hpp"
#include "DatagramStructures.hpp"
#include "SocketsOperation.hpp"
#include "fileManager.hpp"
#include "terminalHandle.hpp"

using namespace std;

// Prototypes
void *userTerminalThread(void*);
void *syncronizationModuleThread(void*);

int main(){
  // Clear server terminals
  clearServerTerminal(TERMINAL_CLIENT_USER_TERMINAL);
  clearServerTerminal(TERMINAL_CLIENT_FOLDER_WATCHER);
  clearServerTerminal(TERMINAL_CLIENT_SYNC_MODULE);

  // Print Welcome message
  cout << "======== DataBag: Take your data wherever you go ========" << endl;
  cout << "\t\t\t==== v" << DATABAG_VERSION <<" ====" << endl;
  cout << "  ** Inicializing ..." << endl;

  // Create INFO Data Communication Sockets
  int info_data_communication_socket = socket(AF_INET, SOCK_STREAM, 0);
  int optval = 1;
  setsockopt(info_data_communication_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  if(info_data_communication_socket == -1)
    pError("\a  ##Error on INFO data communication socket creation!");
  else
    cout << "  ** INFO Data Socket created successfully ..." << endl;

  // Localize Server Host
  struct hostent *dataBag_server_host = gethostbyname("localhost");
  if(dataBag_server_host == NULL)
    pError("\a  ##Failure: Can't localize server address!");   
  else
    cout << "  ** Server address localized successfully!" << endl;

  // Connect Info Data Communication Sockets
  struct sockaddr_in server_sentinel_socket_addr; 
  server_sentinel_socket_addr.sin_family = AF_INET;
  server_sentinel_socket_addr.sin_port = htons(SENTINEL_SOCKET_PORT);
  server_sentinel_socket_addr.sin_addr = *((struct in_addr*)dataBag_server_host->h_addr_list[0]);
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
  optval = 1;
  setsockopt(temp_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


  if(bind(temp_socket, (struct sockaddr *)&temp_socket_addr, sizeof(temp_socket_addr)) == -1)
      pError("\a### Error on TEMP socket binding!");
  else
      cout << "  ** TEMP Socket binded to port "<< CLIENT_RECEIVE_CONNECTION_PORT << " successfully" << endl;
  
  // Listen in TEMP socket to create SYNC Data socket
  if(listen(temp_socket,SYNC_SOCKET_QUEUE_CAPACITY) == -1)
    pError("\a  ##Failure: Can't listen on TEMP socket to recieve SYNC socket!");   
  else
    cout << "  ** TEMP socket listen: Awaiting for server request for SYNC socket ..." << endl;

  // Accept SYNC Data socket
  sockaddr_in server_sync_socket_address;
  socklen_t server_sync_socket_address_len;
  int sync_data_communication_socket = accept(temp_socket, (struct sockaddr *)&server_sync_socket_address, &server_sync_socket_address_len);

  if(sync_data_communication_socket == -1)
    pError("\a  ##Error on SYNC data communication socket creation(acceptance)!");
  else
    cout << "  ** SYNC Data Socket created(accepted) successfully ..." << endl;


  cout << "  ** Connection to server established successfully ..." << endl;
  cout << "  ** Thanks for use DataBag. Type 'login' if you have an account or 'register' to sign in" << endl;

  // Aplication User Terminal
  pthread_t user_terminal_thread;
  ClientStateInformation clientStateInformation;
  clientStateInformation.info_data_communication_socket = info_data_communication_socket;
  clientStateInformation.is_syncronization_active = false;
  clientStateInformation.is_user_logged = false;
  clientStateInformation.is_connected = true;
  strcpy(clientStateInformation.userDataBag.login, "No one logged in");
  strcpy(clientStateInformation.userDataBag.passwd, "No one logged in");

  pthread_create(&user_terminal_thread, NULL, userTerminalThread, (void*)&clientStateInformation);

  // Syncronization Module
  pthread_t syncronization_module_thread;
  clientStateInformation.sync_data_communication_socket = sync_data_communication_socket;
  pthread_create(&syncronization_module_thread, NULL, syncronizationModuleThread, (void*)&clientStateInformation);

  // Close
  pthread_join(user_terminal_thread, NULL);
  close(info_data_communication_socket);
  close(sync_data_communication_socket);
  close(temp_socket);
  return 0;
}

void *userTerminalThread( void *clientStateInformation_arg ){
  ClientStateInformation *clientStateInformation = (ClientStateInformation*)clientStateInformation_arg;
  string command("");

  bool should_close = false;
  while(!should_close){
    cout << " $ ";
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
    }else if(command == "clear"){
      userTerminal_terminals_clear();
    }else if(int pos = command.find("terminal-clear") != string::npos ){
      // pos += 14;
      // command = command.substr(pos, command.size - pos)
      // userTerminal_terminal_clear();
    }else if(command == "terminals-info"){
      userTerminal_terminals_info();
    }else if(command == "quit"){
      clientStateInformation->is_connected = false;
      clientStateInformation->is_user_logged = false;
      clientStateInformation->is_syncronization_active = false;
      should_close = true;
    }else{
      cout << endl << "  Command '" << command << "' not found" << endl;
    }
  }

  return NULL;
}


void *syncronizationModuleThread( void *clientStateInformation_arg ){
  ofstream tty(TERMINAL_CLIENT_SYNC_MODULE, ofstream::out | ofstream::app);
  ClientStateInformation *clientStateInformation = (ClientStateInformation*)clientStateInformation_arg;
  FileMetadata fileMetadata;
  char *buffer;
  int nmr_bytes;
  
  while(!clientStateInformation->is_user_logged){
    if(!clientStateInformation->is_connected)
      return NULL;
  };

  pthread_create(&(clientStateInformation->folder_watcher_thread), NULL, client_folder_watcher_module, (void*)clientStateInformation );
  
  tty << TERMINAL_TEXT_COLOR_WHITE;
  tty << endl << "  ** Synchronization module prepared ..." << endl;
  tty << TERMINAL_TEXT_SETTING_RESET;

  while(clientStateInformation->is_connected){
    
    // Do Nothing if client stop service
    if(clientStateInformation->is_syncronization_active){
      // Read file metadata
      nmr_bytes = socket_read(clientStateInformation->sync_data_communication_socket, &fileMetadata, sizeof(FileMetadata));
      if( nmr_bytes != -1 ){
        if(!fileMetadata.up_to_date_with_server){
          if(fileMetadata.should_delete_file){
            tty << TERMINAL_TEXT_COLOR_YELLOW;
            tty << "   <<< Deletation order received: " << fileMetadata.name << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;

            // Delete file
            string delete_cmd = clientStateInformation->root_folder_path + fileMetadata.name;
            delete_cmd = "rm -r " + delete_cmd;
            system(delete_cmd.c_str());
            system("echo 'deletei_um_file");
          }else{
            tty << TERMINAL_TEXT_COLOR_WHITE;
            tty << "   <<< Metadata received: " << fileMetadata.name << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;
           
            // Read remote file
            buffer = new char[fileMetadata.size];
            nmr_bytes = socket_read(clientStateInformation->sync_data_communication_socket, buffer, fileMetadata.size);

            if( nmr_bytes != -1){
              tty << TERMINAL_TEXT_COLOR_GREEN;
              tty << "   <<< " << fileMetadata.name << " received: " << nmr_bytes << " Bytes" << endl;
              tty << TERMINAL_TEXT_SETTING_RESET;

              // Write in local file
              string file_path = clientStateInformation->root_folder_path + fileMetadata.name;
              ofstream writing_file(file_path, ios::binary | ios::out | ios::trunc);

              if(writing_file.is_open()){
                writing_file.write(buffer, fileMetadata.size);
              }else{
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "  ## Can't open file ";
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << file_path;
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << " of writing " << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;
              }
            }else{
              tty << TERMINAL_TEXT_COLOR_RED;
              tty << "  ## Can't receive ";
              tty << TERMINAL_TEXT_COLOR_CYAN;
              tty << fileMetadata.name;
              tty << TERMINAL_TEXT_COLOR_RED;
              tty << " data by ";
              tty << TERMINAL_TEXT_COLOR_BLUE;
              tty << "SYNC_SOCKET";
              tty << TERMINAL_TEXT_COLOR_RED;
              tty << " from ";
              tty << TERMINAL_TEXT_COLOR_CYAN;
              tty << "server" << endl;
              tty << TERMINAL_TEXT_SETTING_RESET;
            }
            
            delete[] buffer;
          }
        }else{
          // Checking modifications queue
          if(!clientStateInformation->modifications_queue.empty()){
            fileMetadata = clientStateInformation->modifications_queue.front();
            fileMetadata.up_to_date_with_client = false;
            
            // Send metadata to server
            nmr_bytes = write(clientStateInformation->sync_data_communication_socket, (void*)&fileMetadata, sizeof(FileMetadata));
            if( nmr_bytes != -1){
                  tty << TERMINAL_TEXT_COLOR_WHITE;
                tty << "   >>> Metadata sended: " << fileMetadata.name;
                tty << " to ";
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << "server" << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;
            }else{
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "  ## Can't send metadata by ";
                tty << TERMINAL_TEXT_COLOR_BLUE;
                tty << "SYNC_SOCKET";
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << " to ";
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << "server" << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;

            }

            // Read file in sync_dir folder
            string file_path = clientStateInformation->root_folder_path + fileMetadata.name;
            std::ifstream reading_file(file_path);
            if(reading_file.is_open()){
                reading_file.read(buffer, fileMetadata.size);
            }else{
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "  ## Can't read file ";
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << fileMetadata.name;
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << " of sync_dir ";
                tty << TERMINAL_TEXT_SETTING_RESET;
            }

            // Send file to server
            buffer = new char[fileMetadata.size];






              
            clientStateInformation->modifications_queue.pop();
            delete[] buffer;
          }else{
            // All up to date
            fileMetadata.up_to_date_with_client = true;
            // Send metadata to server
            nmr_bytes = write(clientStateInformation->sync_data_communication_socket, (void*)&fileMetadata, sizeof(FileMetadata));
            if( nmr_bytes == -1){
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "  ## Can't send metadata by ";
                tty << TERMINAL_TEXT_COLOR_BLUE;
                tty << "SYNC_SOCKET";
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << " to ";
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << "server" << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;

            }
          }



        }
      }else{
        tty << TERMINAL_TEXT_COLOR_RED;
        tty << "  ## Can't receive metadata by ";
        tty << TERMINAL_TEXT_COLOR_BLUE;
        tty << "SYNC_SOCKET";
        tty << TERMINAL_TEXT_COLOR_RED;
        tty << " from ";
        tty << TERMINAL_TEXT_COLOR_CYAN;
        tty << "server" << endl;
        tty << TERMINAL_TEXT_SETTING_RESET;
      }
    }
  }

  return NULL;
}