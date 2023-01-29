#ifndef __COMMUNICATION_MANAGER_HPP
#define __COMMUNICATION_MANAGER_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "global_definitions.hpp"
#include "ClientDeviceConected.hpp"

using namespace std;

// Provisory Prototypes
void *serverNewDeviceSentinelService(void *);
void *registerNewDataCommunicationSocket(void *);
void *runNewInfoDataCommunicationSocket(void *);
void *runNewSyncDataCommunicationSocket(void *);
bool checkLogin(UserMeineBox );

void *serverNewDeviceSentinelService(void *servers_sentinel_socket_arg){
    int servers_sentinel_socket = *((int*)servers_sentinel_socket_arg);
    
    while(true){
        // Listen
        if( listen(servers_sentinel_socket, SENTINEL_SOCKET_QUEUE_CAPACITY) == -1 ){
            cout << "\a### Failure in listen for connection!" << endl;
        }else{
            cout << ">> Awaiting for connection attempt ..." << endl;
        }

        // Accept
        struct sockaddr_in info_data_communication_socket_addr;
        socklen_t info_data_communication_socket_addr_length;
        int info_data_communication_socket = accept(servers_sentinel_socket, (struct sockaddr *)&info_data_communication_socket_addr, &info_data_communication_socket_addr_length);
        if(info_data_communication_socket == -1){
            cout << "\a### Failure in accept connection!" << endl;
        }else{
            cout << ">> Connection established: " << endl;
            cout << "\t> Port: " << info_data_communication_socket_addr.sin_port << endl;
            cout << "\t> Address: " << info_data_communication_socket_addr.sin_addr.s_addr << endl;
            cout << "\t> Address Family: " << info_data_communication_socket_addr.sin_family << endl;
            cout << "\t> Address Length: " << info_data_communication_socket_addr_length << endl;

            // pthread_t registerNewDataCommunicationSocket_thread;
            ClientDeviceConected clientDeviceConected;
            clientDeviceConected.client_device_address = info_data_communication_socket_addr;
            clientDeviceConected.info_socket_fd = info_data_communication_socket;

            // pthread_create(&registerNewDataCommunicationSocket_thread, NULL, registerNewDataCommunicationSocket,NULL);
            pthread_create(&clientDeviceConected.info_thread, NULL, runNewInfoDataCommunicationSocket,(void*)&clientDeviceConected);
        }
    }

    return NULL;
}

void *registerNewDataCommunicationSocket(void *arg){
    return NULL;
}

void *runNewInfoDataCommunicationSocket(void *clientDeviceConected_arg){
    char data_buffer[DATA_COMMUNICATION_BUFFER_CAPACITY] = "";
    ClientDeviceConected clientDeviceConected = *( (ClientDeviceConected*)clientDeviceConected_arg );
    clientDeviceConected.login_validated = false;
    clientDeviceConected.is_connected = true;
    
    // Sync Socket Creation
    if(connect(clientDeviceConected.sync_socket_fd, (const sockaddr*)&(clientDeviceConected.client_device_address), sizeof(clientDeviceConected.client_device_address) ) == -1){
        cout << "  ## Error to create sync socket:" << endl;
        cout << "  \t# Client Address: " << clientDeviceConected.client_device_address.sin_addr.s_addr << endl;
        cout << "  \t# Client Port: " << clientDeviceConected.client_device_address.sin_port << endl;
        return NULL;    
    }
    
    // Waiting Client Operation Requisition
    int requisition_type;

    while(clientDeviceConected.is_connected){
        // Fetch client requisition
        read(clientDeviceConected.info_socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );

        // Decode requisition type
        switch(requisition_type){
            case CLIENT_REQUEST_LOGIN:
                // Login Validation
                if(!clientDeviceConected.login_validated){
                    read(clientDeviceConected.info_socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
                    strcpy( clientDeviceConected.userMeineBox.login, data_buffer);
                    cout << "User Login Recieved: " << clientDeviceConected.userMeineBox.login << endl;

                    read(clientDeviceConected.info_socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
                    strcpy( clientDeviceConected.userMeineBox.passwd, data_buffer);
                    cout << "User Password Recieved: " << clientDeviceConected.userMeineBox.passwd << endl;

                    if( checkLogin(clientDeviceConected.userMeineBox) ){
                        cout << "  >> Login validated successfully!" << endl;
                        clientDeviceConected.login_validated = true;
                        pthread_create(&clientDeviceConected.sync_thread, NULL, runNewSyncDataCommunicationSocket, (void*)&clientDeviceConected);
                    }else{
                        cout << "\a  >> Invalid Login or Password! Try again ..." << endl;
                    }
                }
                break;
            case CLIENT_REQUEST_REGISTER:

                break;
            case CLIENT_REQUEST_START:
                if(clientDeviceConected.login_validated){
                    if(clientDeviceConected.is_service_active){
                    }
                }
                break;
            case CLIENT_REQUEST_STATUS:
                if(clientDeviceConected.login_validated){
                }
                break;
            case CLIENT_REQUEST_STOP:
                if(clientDeviceConected.login_validated){
                    if(!clientDeviceConected.is_service_active){
                    }
                }
                break;
        }
        
        
        

        
    }

    // Sincronization





    // Close
    close(clientDeviceConected.info_socket_fd);
    return NULL;
}


void *runNewSyncDataCommunicationSocket(void *clientDeviceConected_arg){
    while(1){
        // Receive Sync List

        // Compare the Sync List from device client and server's Sync List for this client 
        if(/*sync list's are diferent*/1){
            if(/*There are diferences in client sync list made before start service */1){
                // Server orders CLIENT to RECEIVE a sync datagram
                // buffer == A sync datagram from a file to needed to be modified
                // write(to_sync_data_socket);
            }else{
            // Server orders CLIENT to SEND a sync datagram
            //read(sync_data_socket);
            // update_server_db
            // update servers sync list for this client 
            }
        }
    }
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