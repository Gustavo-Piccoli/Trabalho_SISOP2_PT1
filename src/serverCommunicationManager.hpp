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
void *runNewDataCommunicationSocket(void *);
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
        struct sockaddr_in data_communication_socket_addr;
        socklen_t data_communication_socket_addr_length;
        int data_communication_socket = accept(servers_sentinel_socket, (struct sockaddr *)&data_communication_socket_addr, &data_communication_socket_addr_length);
        if(data_communication_socket == -1){
            cout << "\a### Failure in accept connection!" << endl;
        }else{
            cout << ">> Connection established: " << endl;
            cout << "\t> Port: " << data_communication_socket_addr.sin_port << endl;
            cout << "\t> Address: " << data_communication_socket_addr.sin_addr.s_addr << endl;
            cout << "\t> Address Family: " << data_communication_socket_addr.sin_family << endl;
            cout << "\t> Address Length: " << data_communication_socket_addr_length << endl;

            pthread_t registerNewDataCommunicationSocket_thread;
            pthread_t runNewDataCommunicationSocket_thread;
            ClientDeviceConected clientDeviceConected;
            clientDeviceConected.client_device_address = data_communication_socket_addr;
            clientDeviceConected.socket_fd = data_communication_socket;

            pthread_create(&registerNewDataCommunicationSocket_thread, NULL, registerNewDataCommunicationSocket,NULL);
            pthread_create(&runNewDataCommunicationSocket_thread, NULL, runNewDataCommunicationSocket,(void*)&clientDeviceConected);
        }
    }

    return NULL;
}

void *registerNewDataCommunicationSocket(void *arg){
    return NULL;
}

void *runNewDataCommunicationSocket(void *clientDeviceConected_arg){
    ClientDeviceConected clientDeviceConected = *( (ClientDeviceConected*)clientDeviceConected_arg );
    char data_buffer[DATA_COMMUNICATION_BUFFER_CAPACITY] = "";
    
    // Login Validation
    clientDeviceConected.login_validated = false;
    while(!clientDeviceConected.login_validated){
        strcpy(data_buffer, "\t >>Login:");
        write(clientDeviceConected.socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
        read(clientDeviceConected.socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
        strcpy( clientDeviceConected.userMeineBox.login, data_buffer);
        cout << "User Login Recieved: " << clientDeviceConected.userMeineBox.login << endl;

        strcpy(data_buffer, "\t >>Password:");
        write(clientDeviceConected.socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
        read(clientDeviceConected.socket_fd, (void*)&data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY );
        strcpy( clientDeviceConected.userMeineBox.passwd, data_buffer);
        cout << "User Password Recieved: " << clientDeviceConected.userMeineBox.passwd << endl;

        if( checkLogin(clientDeviceConected.userMeineBox) ){
            cout << "  >> Login validated successfully!" << endl;
            clientDeviceConected.login_validated = true;
        }else{
            cout << "\a  >> Invalid Login or Password! Try again ..." << endl;
        }
    }

    // Sincronization





    // Close
    close(clientDeviceConected.socket_fd);
    return NULL;
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

//     userDataToCheck_bytes_read = read( data_communication_socket, &userDataToCheck, sizeof(UserMeineBox) );

//     if( userDataToCheck_bytes_read < 0 ){
//         cout << "Problems in read user login data!" << endl;
//     }else{
//         if( 1 ) ;
//     }
#endif