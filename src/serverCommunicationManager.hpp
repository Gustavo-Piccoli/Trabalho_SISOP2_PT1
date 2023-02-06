#ifndef __COMMUNICATION_MANAGER_HPP
#define __COMMUNICATION_MANAGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h> 
#include <dirent.h> 

#include "global_definitions.hpp"
#include "ClientDeviceConected.hpp"
#include "DatagramStructures.hpp"
#include "fileManager.hpp"
#include "SocketsOperation.hpp"

using namespace std;

// Provisory Prototypes
void *serverSentinelModule(void *) __attribute__((optimize(0)));
void *registerNewDataCommunicationSocket(void *);
void *runNewInfoDataCommunicationSocket(void *);
void *runNewSyncDataCommunicationSocket(void *);
bool checkLogin(UserDataBag);
void disconect_client_device(ClientDeviceConnected *);
void *monitore_if_server_should_close(void *);

void *serverSentinelModule(void *servers_sentinel_socket_arg){
    int servers_sentinel_socket = *((int *)servers_sentinel_socket_arg);
    struct sockaddr_in info_data_communication_socket_addr;
    socklen_t info_data_communication_socket_addr_length = sizeof(struct sockaddr_in);
    int info_data_communication_socket;
    ofstream tty(TERMINAL_SERVER_SENTINEL, ofstream::out | ofstream::app);

  
    while(true) {
        // Listen
        if (listen(servers_sentinel_socket, SENTINEL_SOCKET_QUEUE_CAPACITY) == -1){
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << "\a### Failure in listen for connection!" << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;
        }else{
            tty << TERMINAL_TEXT_COLOR_WHITE;
            tty << "  ** Awaiting for connection attempt in port ..." << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;
        }

        // Accept
        info_data_communication_socket = accept(servers_sentinel_socket, (struct sockaddr *)&info_data_communication_socket_addr, &info_data_communication_socket_addr_length);

        if (info_data_communication_socket == -1){
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << "\a### Failure in accept connection!" << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;
        }else{
            tty << TERMINAL_TEXT_COLOR_GREEN;
            tty << "\t  ** Connection established: " << endl;
            tty << "\t\t* Port: " << info_data_communication_socket_addr.sin_port << endl;
            tty << "\t\t* Address: " << info_data_communication_socket_addr.sin_addr.s_addr << endl;
            tty << "\t\t* Address Family: " << info_data_communication_socket_addr.sin_family << endl;
            tty << "\t\t* Address Length: " << info_data_communication_socket_addr_length << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;

            // pthread_t registerNewDataCommunicationSocket_thread;
            ClientDeviceConnected clientDeviceConnected;
            clientDeviceConnected.client_device_address_info = info_data_communication_socket_addr;
            clientDeviceConnected.info_socket_fd = info_data_communication_socket;

            // pthread_create(&registerNewDataCommunicationSocket_thread, NULL, registerNewDataCommunicationSocket,NULL);
            pthread_create(&clientDeviceConnected.info_thread, NULL, runNewInfoDataCommunicationSocket, (void *)&clientDeviceConnected);
        }
        
    }

    return NULL;
}

void *registerNewDataCommunicationSocket(void *arg){
    return NULL;
}

void *runNewInfoDataCommunicationSocket(void *clientDeviceConected_arg){
    ClientDeviceConnected clientDeviceConnected = *((ClientDeviceConnected *)clientDeviceConected_arg);
    clientDeviceConnected.login_validated = false;
    clientDeviceConnected.is_connected = true;
    ofstream tty(TERMINAL_SERVER_INFO_SOCKET, ofstream::out | ofstream::app);

    // SYNC Data Socket Creation
    clientDeviceConnected.sync_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientDeviceConnected.sync_socket_fd == -1){
        tty << TERMINAL_TEXT_COLOR_RED;
        tty << "\a  ##Error on SYNC data communication socket creation!" << endl;
        tty << TERMINAL_TEXT_SETTING_RESET;
        disconect_client_device(&clientDeviceConnected);
        return NULL;
    }else{
        tty << TERMINAL_TEXT_SETTING_RESET;
        tty << "  ** Sync Data Socket created successfully ..." << endl;
    }
    
    int optval = 1;
    setsockopt(clientDeviceConnected.sync_socket_fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));


    clientDeviceConnected.client_device_address_sync = clientDeviceConnected.client_device_address_info;
    clientDeviceConnected.client_device_address_sync.sin_port = htons(CLIENT_RECEIVE_CONNECTION_PORT);
    bzero(&(clientDeviceConnected.client_device_address_sync.sin_zero), 8 );

    // SYNC Data Socket Connection
    int connect_result, i=3;
    do{
        connect_result = connect(clientDeviceConnected.sync_socket_fd, (struct sockaddr *)&(clientDeviceConnected.client_device_address_sync), sizeof(clientDeviceConnected.client_device_address_sync));
        i--;
        if(connect_result == 0)
            i = 0;
    }while(i);
            
    if (connect_result == -1){
        tty << TERMINAL_TEXT_COLOR_RED;
        tty << "  ## Error to connect sync socket:" << endl;
        tty << "  \t# Client Address: ";
        tty << TERMINAL_TEXT_COLOR_CYAN;
        tty << clientDeviceConnected.client_device_address_sync.sin_addr.s_addr << endl;
        tty << TERMINAL_TEXT_SETTING_RESET;
        tty << "  \t# Client Port: ";
        tty << TERMINAL_TEXT_COLOR_CYAN;
        tty << clientDeviceConnected.client_device_address_sync.sin_port << endl;
        tty << TERMINAL_TEXT_SETTING_RESET;
        disconect_client_device(&clientDeviceConnected);
        return NULL;
    }else{
        tty << TERMINAL_TEXT_SETTING_RESET;
        tty << "  ** Sync Data Socket connected successfully ..." << endl;
    }

    // Waiting Client Operation Requisition
    ClientRequestDatagram clientRequestDatagram;
    ServerRequestResponseDatagram serverRequestResponseDatagram;
    int nmr_bytes;
    while (clientDeviceConnected.is_connected){
        // Fetch client requisition
        nmr_bytes = socket_read(clientDeviceConnected.info_socket_fd, &clientRequestDatagram, REQUEST_DATAGRAM_SIZE);
        if( nmr_bytes == -1 ){
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << "  ## Can't receive request";
            tty << TERMINAL_TEXT_COLOR_CYAN;
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << " request by ";
            tty << TERMINAL_TEXT_COLOR_BLUE;
            tty << "INFO_SOCKET";
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << " from user ";
            tty << TERMINAL_TEXT_COLOR_CYAN;
            tty << clientDeviceConnected.userDataBag.login;
            tty << TERMINAL_TEXT_COLOR_RED;
            tty << "!" << endl;
            tty << TERMINAL_TEXT_SETTING_RESET;
        }else{
            tty << TERMINAL_TEXT_COLOR_WHITE;
            tty << endl << "   <<< Request Received. Type ";
            tty << TERMINAL_TEXT_COLOR_CYAN;
            tty << clientRequestDatagram.requisition_type;
            tty << TERMINAL_TEXT_SETTING_RESET;
            tty << ": ";
            if(clientRequestDatagram.requisition_type < 0 || clientRequestDatagram.requisition_type > MAX_CLIENT_REQUEST_CODE){
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "UKNOWN request type" << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;
            }else{
                tty << TERMINAL_TEXT_COLOR_CYAN;
                tty << REQUESTS_NAMES[clientRequestDatagram.requisition_type] << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;

                // Decode requisition type
                switch (clientRequestDatagram.requisition_type){
                case CLIENT_REQUEST_QUIT:                    
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "  ## Can't receive request by";
                    tty << TERMINAL_TEXT_COLOR_BLUE;
                    tty << "INFO_SOCKET";
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << " from user ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected.userDataBag.login;
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "!" << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                    
                    disconect_client_device(&clientDeviceConnected);
                    break;
                case CLIENT_REQUEST_LOGIN:
                    // Login Validation
                    if (!clientDeviceConnected.login_validated){
                        tty << "User Login Recieved: " << clientRequestDatagram.userDataBag.login << endl;
                        tty << "User Password Recieved: " << clientRequestDatagram.userDataBag.passwd << endl;

                        if (checkLogin(clientRequestDatagram.userDataBag)){
                            tty << "  ** Login validated successfully!" << endl;
                            clientDeviceConnected.login_validated = true;
                            clientDeviceConnected.userDataBag = clientRequestDatagram.userDataBag;
                            
                            serverRequestResponseDatagram.was_login_validated_successfully = true;
                            serverRequestResponseDatagram.userDataBag = clientDeviceConnected.userDataBag;

                            // Preparing DB
                            clientDeviceConnected.db_folder_path = DB_USERS_DATA_FOLDER;
                            clientDeviceConnected.db_folder_path += clientDeviceConnected.userDataBag.login;

                            //          Fix HOME folder name
                            string home_dir = getenv("HOME");
                            home_dir += '/';
                            string home_sign = "~/";
                            auto last_ocur = 0;
                            while((last_ocur = clientDeviceConnected.db_folder_path.find(home_sign,last_ocur)) != string::npos){
                                cout << clientDeviceConnected.db_folder_path << endl;
                                clientDeviceConnected.db_folder_path.replace( last_ocur, home_sign.length(), home_dir );
                                last_ocur += home_dir.length();
                            }

                            struct stat st;
                            int status = stat(clientDeviceConnected.db_folder_path.c_str(), &st);
                            
                            if(status != 0){
                                tty << "  ** DB: User Data Folder Created: " << clientDeviceConnected.db_folder_path << endl;
                                // mkdir(clientDeviceConnected.db_folder_path.c_str(), DB_USERS_DATA_FOLDER_PERMISSION, true);
                                string cmd = "mkdir -p ";   // create also parents folders if necessary
                                cmd += clientDeviceConnected.db_folder_path;                        
                                system(cmd.c_str());
                            }else{
                                tty << "  ** DB: Folder " << clientDeviceConnected.db_folder_path << " prepared ..." << endl;
                            }
                            tty << "  ** Client DB Folder prepared: " << clientDeviceConnected.userDataBag.login << endl;
                            tty << "  \t> Path: ";
                            tty << TERMINAL_TEXT_COLOR_CYAN;
                            tty << clientDeviceConnected.db_folder_path << endl;
                            tty << TERMINAL_TEXT_SETTING_RESET;

                            pthread_create(&(clientDeviceConnected.file_monitoring_module_thread), NULL, server_folder_watcher_module, (void*)&clientDeviceConnected);
                            pthread_create(&clientDeviceConnected.sync_thread, NULL, runNewSyncDataCommunicationSocket, (void *)&clientDeviceConnected);
                            
                        }
                        else
                        {
                            tty << "\a  << Invalid Login or Password!" << endl;
                        }
                    }
                    break;
                case CLIENT_REQUEST_REGISTER:
                    // Register new user
                    break;
                case CLIENT_REQUEST_START:
                    if (clientDeviceConnected.login_validated){
                        serverRequestResponseDatagram.service_activation_is_already_this = clientDeviceConnected.is_service_active;
                        clientDeviceConnected.is_service_active = true;
                        serverRequestResponseDatagram.service_activation_state = true;
                                                
                        tty << TERMINAL_TEXT_SETTING_RESET;
                        tty << "  ** ";
                        tty << TERMINAL_TEXT_COLOR_GREEN;
                        tty << "Successfully ";
                        tty << TERMINAL_TEXT_SETTING_RESET;
                        tty << "started SYNC service to user ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << clientDeviceConnected.userDataBag.login << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;
                    }
                    break;
                case CLIENT_REQUEST_STATUS:
                    if (clientDeviceConnected.login_validated){
                    }
                    break;
                case CLIENT_REQUEST_STOP:
                    if (clientDeviceConnected.login_validated)
                    {
                        if (!clientDeviceConnected.is_service_active)
                        {
                        }
                    }
                    break;
                }
                
                // Answer Client Request
                nmr_bytes = write(clientDeviceConnected.info_socket_fd, (void *)&serverRequestResponseDatagram, REQUEST_RESPONSE_DATAGRAM_SIZE);  
                if( nmr_bytes == -1 ){
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "  ## Can't answer client request by";
                    tty << TERMINAL_TEXT_COLOR_BLUE;
                    tty << " SYNC_SOCKET";
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << " to user ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected.userDataBag.login << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                }  
            }
        }
    }

    return NULL;
}

void *runNewSyncDataCommunicationSocket(void *clientDeviceConected_arg){
    ClientDeviceConnected *clientDeviceConnected = (ClientDeviceConnected *)clientDeviceConected_arg;
    FileMetadata fileMetadata;
    char *buffer;
    int nmr_bytes;
    ofstream tty(TERMINAL_SERVER_SYNC_MODULE, ofstream::out | ofstream::app);

    // Copying server folder to user folder
    struct dirent *folder_element_pointer;  // Pointer for directory entry 
    DIR *directory = opendir(clientDeviceConnected->db_folder_path.c_str()); 
  
    if (directory == NULL){ 
        tty << TERMINAL_TEXT_COLOR_RED;
        tty << "  ## Can't open DB user directory!" << endl; 
        tty << "  ## " << clientDeviceConnected->db_folder_path << " tried ..." << endl; 
        tty << TERMINAL_TEXT_SETTING_RESET;
        return NULL; 
    }else{
        tty << "  ** Assigning server contents in ";
        tty << TERMINAL_TEXT_COLOR_CYAN;
        tty << clientDeviceConnected->userDataBag.login;
        tty << TERMINAL_TEXT_SETTING_RESET;
        tty << " broadcast queue  ..." << endl;
        tty << "  ** DB folder: ";
        tty << TERMINAL_TEXT_COLOR_CYAN;
        tty << clientDeviceConnected->db_folder_path << endl;
        tty << TERMINAL_TEXT_SETTING_RESET;
    }
    
    while ((folder_element_pointer = readdir(directory)) != NULL){
        if(folder_element_pointer->d_type == DT_REG){
            strcpy(fileMetadata.name, folder_element_pointer->d_name);
            std::ifstream reading_file( clientDeviceConnected->db_folder_path + "/" + fileMetadata.name, ios::binary );
            if( !reading_file.is_open() ){
                tty << TERMINAL_TEXT_COLOR_RED;
                tty << "  ## Cant open " << clientDeviceConnected->db_folder_path + "/" + fileMetadata.name << endl;
                tty << TERMINAL_TEXT_SETTING_RESET;
            }else{
                struct stat file_info;                
                stat( (clientDeviceConnected->db_folder_path + "/" + fileMetadata.name).c_str(), &file_info);
                fileMetadata.up_to_date_with_server = false;
                fileMetadata.should_delete_file = false;
                fileMetadata.size = file_info.st_size;

                clientDeviceConnected->modified_files_queue.push(fileMetadata);
            }
        }
    }

    tty << "  ** Syncronization running ..." << endl;
    while(clientDeviceConnected->is_connected ){
        if(clientDeviceConnected->is_service_active){
            if(!clientDeviceConnected->modified_files_queue.empty()){
                // To broadcast file modifications to client device
                fileMetadata = clientDeviceConnected->modified_files_queue.front();
                fileMetadata.up_to_date_with_server = false;
                buffer = new char[fileMetadata.size];
                
                // Send metadata to client
                nmr_bytes = write(clientDeviceConnected->sync_socket_fd, (void*) &fileMetadata, sizeof(fileMetadata));
                if(nmr_bytes != -1){
                    tty << TERMINAL_TEXT_COLOR_WHITE;
                    tty << "   >>> Metadata sended: " << fileMetadata.name;
                    tty << " to ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected->userDataBag.login << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                }else{
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "  ## Can't send metadata by ";
                    tty << TERMINAL_TEXT_COLOR_BLUE;
                    tty << "SYNC_SOCKET";
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << " to user ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected->userDataBag.login << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                }
                
                if(!fileMetadata.should_delete_file){
                    // Read DataBase File
                    std::ifstream reading_file(clientDeviceConnected->db_folder_path + "/" + fileMetadata.name);
                    if(reading_file.is_open()){
                        reading_file.read(buffer, fileMetadata.size);
                    }else{
                        tty << TERMINAL_TEXT_COLOR_RED;
                        tty << "  ## Can't read file ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << fileMetadata.name;
                        tty << TERMINAL_TEXT_COLOR_RED;
                        tty << " of user DB ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << clientDeviceConnected->userDataBag.login << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;
                    }

                    
                    // Send Data File to Cliente
                    nmr_bytes = write(clientDeviceConnected->sync_socket_fd, (void*)buffer, fileMetadata.size);
                    if( nmr_bytes != -1 ){
                        tty << TERMINAL_TEXT_COLOR_GREEN;
                        tty << "   >>> " << fileMetadata.name << "(" <<  fileMetadata.size << " Bytes) "<< " sended to ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << clientDeviceConnected->userDataBag.login << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;
                    }else{
                        tty << TERMINAL_TEXT_COLOR_RED;
                        tty << "  ## Can't send ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << fileMetadata.name;
                        tty << TERMINAL_TEXT_COLOR_RED;
                        tty << " data by ";
                        tty << TERMINAL_TEXT_COLOR_BLUE;
                        tty << "SYNC_SOCKET";
                        tty << TERMINAL_TEXT_COLOR_RED;
                        tty << " to user ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << clientDeviceConnected->userDataBag.login << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;
                    }

                    delete[] buffer;
                }

                clientDeviceConnected->modified_files_queue.pop();
            }else{
                // Request file modifications in client devices side
                // Send metadata to client
                fileMetadata.up_to_date_with_server = true;
                nmr_bytes = write(clientDeviceConnected->sync_socket_fd, (void*) &fileMetadata, sizeof(fileMetadata));
                if(nmr_bytes == -1){
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "  ## Can't send metadata by ";
                    tty << TERMINAL_TEXT_COLOR_BLUE;
                    tty << "SYNC_SOCKET";
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << " to user ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected->userDataBag.login << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                }

                // Receive metadata from client
                nmr_bytes = socket_read(clientDeviceConnected->sync_socket_fd, &fileMetadata, sizeof(fileMetadata));
                
                if(nmr_bytes != -1){
                    if(!fileMetadata.up_to_date_with_client){
                        tty << TERMINAL_TEXT_COLOR_WHITE;
                        tty << "   <<< Metadata received: " << fileMetadata.name << " from ";
                        tty << TERMINAL_TEXT_COLOR_CYAN;
                        tty << clientDeviceConnected->userDataBag.login << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;

                        // Receive file from client
                        buffer = new char[fileMetadata.size];
                        nmr_bytes = socket_read(clientDeviceConnected->sync_socket_fd, buffer, fileMetadata.size);
                        if(nmr_bytes != -1 ){
                            tty << TERMINAL_TEXT_COLOR_GREEN;
                            tty << "   <<< " << fileMetadata.name << "(" << nmr_bytes << " Bytes) received: " << clientDeviceConnected->userDataBag.login << endl;
                            tty << TERMINAL_TEXT_SETTING_RESET;

                            // Writing file in db
                            std::ofstream writing_file(clientDeviceConnected->db_folder_path + "/" + fileMetadata.name, ofstream::binary | ofstream::out | ofstream::trunc);
                            writing_file.write(buffer, fileMetadata.size);

                                if (writing_file.fail()) {
                                tty << TERMINAL_TEXT_COLOR_RED;
                                tty << "  ## Can't write file ";
                                tty << TERMINAL_TEXT_COLOR_CYAN;
                                tty << fileMetadata.name;
                                tty << TERMINAL_TEXT_COLOR_RED;
                                tty << " in DB folder of user ";
                                tty << TERMINAL_TEXT_COLOR_CYAN;
                                tty << clientDeviceConnected->userDataBag.login << endl;
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
                            tty << " from user ";
                            tty << TERMINAL_TEXT_COLOR_CYAN;
                            tty << clientDeviceConnected->userDataBag.login << endl;
                            tty << TERMINAL_TEXT_COLOR_RED;
                            tty << "!" << endl;
                            tty << TERMINAL_TEXT_SETTING_RESET;
                        }

                        delete[] buffer;
                    }
                }else{
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << "  ## Can't receive metadata by ";
                    tty << TERMINAL_TEXT_COLOR_BLUE;
                    tty << "SYNC_SOCKET";
                    tty << TERMINAL_TEXT_COLOR_RED;
                    tty << " from user ";
                    tty << TERMINAL_TEXT_COLOR_CYAN;
                    tty << clientDeviceConnected->userDataBag.login << endl;
                    tty << TERMINAL_TEXT_SETTING_RESET;
                }
            }
        }
    }
  
    closedir(directory);     
    return NULL;
}

void disconect_client_device(ClientDeviceConnected *clientDeviceConnected){
    // Close socket's
    close(clientDeviceConnected->info_socket_fd);
    close(clientDeviceConnected->sync_socket_fd);

    // Reset bool fields
    clientDeviceConnected->is_service_active = false;
    clientDeviceConnected->login_validated = false;
    clientDeviceConnected->is_connected = false;

    // Printing
    ofstream tty(TERMINAL_SERVER_INFO_SOCKET, ofstream::out | ofstream::app);
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << "Device ";
    tty << TERMINAL_TEXT_COLOR_CYAN;
    tty << clientDeviceConnected->client_device_address_info.sin_addr.s_addr;
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << " : ";
    tty << TERMINAL_TEXT_COLOR_BLUE;
    tty << clientDeviceConnected->client_device_address_info.sin_port;
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << " for user ";
    tty << TERMINAL_TEXT_COLOR_CYAN;
    tty << clientDeviceConnected->userDataBag.login;
    tty << TERMINAL_TEXT_COLOR_RED;
    tty << " disconnected";
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << " !" << endl;
}

bool checkLogin(UserDataBag userDataBag){
    // acessa db
    // while (não existe login no db || !(encontrou login no db)){
    //     read no db dos users
    //     checa se login é igual ao fornecido
    //         se for igual seta login como encontrado
    // }

    // se existe login no db e passwd confere
    return true;
    // else
    //     return false;
//     // Check Login
//     UserDataBag userDataToCheck;
//     size_t userDataToCheck_bytes_read;

//     userDataToCheck_bytes_read = read( info_data_communication_socket, &userDataToCheck, sizeof(UserDataBag) );

//     if( userDataToCheck_bytes_read < 0 ){
//         cout << "Problems in read user login data!" << endl;
//     }else{
//         if( 1 ) ;
//     }
}

void *monitore_if_server_should_close(void * close_flag_arg){
    struct stat st;
    bool *close_flag = (bool*)close_flag_arg;
    cout << "  ** Server Close Intention Monitor running ..." << endl;
    while(stat(FILE_OR_FOLDER_TO_CLOSE_SERVER, &st) != 0){};
    
    *close_flag = true;
    return NULL;
}

#endif