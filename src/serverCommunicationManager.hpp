#ifndef __COMMUNICATION_MANAGER_HPP
#define __COMMUNICATION_MANAGER_HPP

#include <iostream>
#include <string>
#include <filesystem>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "global_definitions.hpp"
#include "ClientDeviceConected.hpp"
#include "DatagramStructures.hpp"
#include "fileManager.hpp"

using namespace std;

// Provisory Prototypes
void *serverNewDeviceSentinelService(void *);
void *registerNewDataCommunicationSocket(void *);
void *runNewInfoDataCommunicationSocket(void *);
void *runNewSyncDataCommunicationSocket(void *);
bool checkLogin(UserMeineBox);
void disconect_client_device(ClientDeviceConected *);


void *serverNewDeviceSentinelService(void *servers_sentinel_socket_arg){
    int servers_sentinel_socket = *((int *)servers_sentinel_socket_arg);
    struct sockaddr_in info_data_communication_socket_addr;
    socklen_t info_data_communication_socket_addr_length = sizeof(struct sockaddr_in);
    int info_data_communication_socket;

    while(true) {
        // Listen
        if (listen(servers_sentinel_socket, SENTINEL_SOCKET_QUEUE_CAPACITY) == -1){
            cout << TERMINAL_TEXT_COLOR_RED;
            cout << "\a### Failure in listen for connection!" << endl;
            cout << TERMINAL_TEXT_SETTING_RESET;
        }else{
            cout << TERMINAL_TEXT_COLOR_GREEN;
            cout << ">> Awaiting for connection attempt ..." << endl;
            cout << TERMINAL_TEXT_SETTING_RESET;
        }

        // Accept
        info_data_communication_socket = accept(servers_sentinel_socket, (struct sockaddr *)&info_data_communication_socket_addr, &info_data_communication_socket_addr_length);

        if (info_data_communication_socket == -1){
            cout << "\a### Failure in accept connection!" << endl;
        }else{
            cout << ">> Connection established: " << endl;
            cout << "\t> Port: " << info_data_communication_socket_addr.sin_port << endl;
            cout << "\t> Address: " << info_data_communication_socket_addr.sin_addr.s_addr << endl;
            cout << "\t> Address Family: " << info_data_communication_socket_addr.sin_family << endl;
            cout << "\t> Address Length: " << info_data_communication_socket_addr_length << endl;
            cout << TERMINAL_TEXT_SETTING_RESET;

            // pthread_t registerNewDataCommunicationSocket_thread;
            ClientDeviceConected clientDeviceConected;
            clientDeviceConected.client_device_address_info = info_data_communication_socket_addr;
            clientDeviceConected.info_socket_fd = info_data_communication_socket;

            // pthread_create(&registerNewDataCommunicationSocket_thread, NULL, registerNewDataCommunicationSocket,NULL);
            pthread_create(&clientDeviceConected.info_thread, NULL, runNewInfoDataCommunicationSocket, (void *)&clientDeviceConected);
        }
        
    }

    return NULL;
}

void *registerNewDataCommunicationSocket(void *arg){
    return NULL;
}

void *runNewInfoDataCommunicationSocket(void *clientDeviceConected_arg){
    ClientDeviceConected clientDeviceConected = *((ClientDeviceConected *)clientDeviceConected_arg);
    clientDeviceConected.login_validated = false;
    clientDeviceConected.is_connected = true;

    // SYNC Data Socket Creation
    clientDeviceConected.sync_socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientDeviceConected.sync_socket_fd == -1)
        pError("\a  ##Error on SYNC data communication socket creation!");
    else
      cout << "  >> Sync Data Socket created successfully ..." << endl;
    
    clientDeviceConected.client_device_address_sync = clientDeviceConected.client_device_address_info;
    clientDeviceConected.client_device_address_sync.sin_port = htons(CLIENT_RECEIVE_CONNECTION_PORT);
    bzero(&(clientDeviceConected.client_device_address_sync.sin_zero), 8 );

    // SYNC Data Socket Connection
    int connect_result, i=3;
    do{
        connect_result = connect(clientDeviceConected.sync_socket_fd, (struct sockaddr *)&(clientDeviceConected.client_device_address_sync), sizeof(clientDeviceConected.client_device_address_sync));
        i--;
        if(connect_result == 0)
            i = 0;
    }while(i);
            
    if (connect_result == -1){
        cout << "  ## Error to connect sync socket:" << endl;
        cout << "  \t# Client Address: " << clientDeviceConected.client_device_address_sync.sin_addr.s_addr << endl;
        cout << "  \t# Client Port: " << clientDeviceConected.client_device_address_sync.sin_port << endl;
        disconect_client_device(&clientDeviceConected);
        return NULL;
    }else{
        cout << "  >> Sync Data Socket connected successfully ..." << endl;
    }

    // Waiting Client Operation Requisition
    ClientRequestDatagram clientRequestDatagram;
    ServerRequestResponseDatagram serverRequestResponseDatagram;
    
    while (clientDeviceConected.is_connected){
        // Fetch client requisition
        read(clientDeviceConected.info_socket_fd, (void *)&clientRequestDatagram, REQUEST_DATAGRAM_SIZE);
        cout << "Request Received. Type: " << clientRequestDatagram.requisition_type << endl;
        float h = 0;

        // Decode requisition type
        switch (clientRequestDatagram.requisition_type){
        case CLIENT_REQUEST_QUIT:
            break;
        case CLIENT_REQUEST_LOGIN:
            // Login Validation
            if (!clientDeviceConected.login_validated){
                cout << "User Login Recieved: " << clientRequestDatagram.userMeineBox.login << endl;
                cout << "User Password Recieved: " << clientRequestDatagram.userMeineBox.passwd << endl;

                if (checkLogin(clientRequestDatagram.userMeineBox)){
                    cout << "  >> Login validated successfully!" << endl;
                    clientDeviceConected.login_validated = true;
                    clientDeviceConected.userMeineBox = clientRequestDatagram.userMeineBox;
                    
                    serverRequestResponseDatagram.was_login_validated_successfully = true;
                    serverRequestResponseDatagram.userMeineBox = clientDeviceConected.userMeineBox;

                    // Answer Client Request
                    write(clientDeviceConected.info_socket_fd, (void *)&serverRequestResponseDatagram, REQUEST_RESPONSE_DATAGRAM_SIZE);

                    pthread_create(&clientDeviceConected.sync_thread, NULL, runNewSyncDataCommunicationSocket, (void *)&clientDeviceConected);
                }
                else
                {
                    cout << "\a  >> Invalid Login or Password!" << endl;
                }
            }
            break;
        case CLIENT_REQUEST_REGISTER:
            // Register new user
            if (!clientDeviceConected.login_validated){
                cout << "User Login Recieved: " << clientRequestDatagram.userMeineBox.login << endl;
                cout << "User Password Recieved: " << clientRequestDatagram.userMeineBox.passwd << endl;

                if (checkLogin(clientRequestDatagram.userMeineBox)){
                    cout << "  >> Login validated successfully!" << endl;
                    clientDeviceConected.login_validated = true;
                    clientDeviceConected.userMeineBox = clientRequestDatagram.userMeineBox;
                    
                    serverRequestResponseDatagram.was_login_validated_successfully = true;
                    serverRequestResponseDatagram.userMeineBox = clientDeviceConected.userMeineBox;

                    pthread_create(&clientDeviceConected.sync_thread, NULL, runNewSyncDataCommunicationSocket, (void *)&clientDeviceConected);
                }
                else
                {
                    cout << "\a  >> Invalid Login or Password!" << endl;
                }
            }
            break;
        case CLIENT_REQUEST_START:
            if (clientDeviceConected.login_validated){
                if(clientDeviceConected.is_service_active == true)
                    serverRequestResponseDatagram.service_activation_is_already_this = true;
                clientDeviceConected.is_service_active = true;
                serverRequestResponseDatagram.service_activation_state = true;
            }else{
                serverRequestResponseDatagram.service_activation_state = false;
                serverRequestResponseDatagram.service_activation_is_already_this = false;
            }
            break;
        case CLIENT_REQUEST_STATUS:
            if (clientDeviceConected.login_validated){
            }
            break;
        case CLIENT_REQUEST_STOP:
            if (clientDeviceConected.login_validated)
            {
                if (!clientDeviceConected.is_service_active)
                {
                }
            }
            break;
        case CLIENT_REQUEST_UPLOAD:
            if (clientDeviceConected.login_validated)
            {
                if (!clientDeviceConected.is_service_active)
                {
                    //recv()
                }
            }
            break;
        }

        // Answer Client Request
        write(clientDeviceConected.info_socket_fd, (void *)&serverRequestResponseDatagram, REQUEST_RESPONSE_DATAGRAM_SIZE);    
    }

    return NULL;
}

void *runNewSyncDataCommunicationSocket(void *clientDeviceConected_arg){
    ClientDeviceConected *clientDeviceConected = (ClientDeviceConected *)clientDeviceConected_arg;
    SyncList clientSyncList;
    SyncList serverSyncList;
    
    while (clientDeviceConected->is_connected){
        // Froze if client stop service
        while (!clientDeviceConected->is_service_active){};
        
        /// Synchronization
        // Receive Sync List

        // Compare the Sync List from device client and server's Sync List for this client
        int greater_list_len = std::max(clientSyncList.num_files, serverSyncList.num_files);
        int *changes_to_do = new int[greater_list_len];

        changes_to_do = compare_sync_lists(&clientSyncList, &serverSyncList);

        // Send files
        string file_name;
        int file_size;
        for( int i = 0 ; i < greater_list_len ; i++ ){
            if(changes_to_do[i] != -1){
                file_name = serverSyncList.files[i].name;
                // filesystem::path file_path{file_name.};

                // file_size = 
                // read()
            }
        }

        // Receive files


        // if (/*sync list's are diferent*/ 1)
        // {
        //     if (/*There are diferences in client sync list made before start service */ 1)
        //     {
        //         // Server orders CLIENT to RECEIVE a sync datagram
        //         // buffer == A sync datagram from a file to needed to be modified
        //         // write(to_sync_data_socket);
        //     }
        //     else
        //     {
        //         // Server orders CLIENT to SEND a sync datagram
        //         // read(sync_data_socket);
        //         // update_server_db
        //         // update servers sync list for this client
        //     }
        // }
        delete changes_to_do;
    }

    return NULL;
}

void disconect_client_device(ClientDeviceConected *clientDeviceConected){
    // Reset bool fields
    clientDeviceConected->is_service_active = false;
    clientDeviceConected->login_validated = false;
    clientDeviceConected->is_connected = false;
    
    // Close socket's
    close(clientDeviceConected->info_socket_fd);
    close(clientDeviceConected->sync_socket_fd);
}

bool checkLogin(UserMeineBox userMeineBox){
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
}
//     // Check Login
//     UserMeineBox userDataToCheck;
//     size_t userDataToCheck_bytes_read;

//     userDataToCheck_bytes_read = read( info_data_communication_socket, &userDataToCheck, sizeof(UserMeineBox) );

//     if( userDataToCheck_bytes_read < 0 ){
//         cout << "Problems in read user login data!" << endl;
//     }else{
//         if( 1 ) ;
//     }
#endif