#ifndef __CLIENT_DEVICE_CONECTED_HPP
#define __CLIENT_DEVICE_CONECTED_HPP

#include <queue>
#include <pthread.h>
#include <netinet/in.h>

#include "userDataBag.hpp"
#include "DatagramStructures.hpp"

typedef struct ClientDeviceConnected{
    pthread_t info_thread;
    pthread_t sync_thread;
    pthread_t file_monitoring_module_thread;
    
    int info_socket_fd;
    int sync_socket_fd;
    sockaddr_in client_device_address_info;
    sockaddr_in client_device_address_sync;
    
    UserDataBag userDataBag;
    
    bool login_validated;
    bool is_connected;
    bool is_service_active;

    string db_folder_path;

    queue<FileMetadata> modified_files_queue;
}ClientDeviceConnected;

#endif