#ifndef __CLIENT_DEVICE_CONECTED_HPP
#define __CLIENT_DEVICE_CONECTED_HPP

#include <pthread.h>
#include <netinet/in.h>
#include "userMeineBox.hpp"

typedef struct ClientDeviceConected{
    pthread_t info_thread;
    pthread_t sync_thread;
    
    int info_socket_fd;
    int sync_socket_fd;
    sockaddr_in client_device_address_info;
    sockaddr_in client_device_address_sync;
    
    UserMeineBox userMeineBox;
    
    bool login_validated;
    bool is_connected;
    bool is_service_active;
}ClientDeviceConected;

#endif