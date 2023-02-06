#ifndef __USER_TERMINAL_THREAD_INFO_HPP
#define __USER_TERMINAL_THREAD_INFO_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <queue>
#include <pthread.h>
#include "userDataBag.hpp"
#include "DatagramStructures.hpp"

typedef struct ClientStateInformation{
    int info_data_communication_socket;
    int sync_data_communication_socket;
    bool is_syncronization_active;
    bool is_user_logged;
    bool is_connected;

    UserDataBag userDataBag;
    ofstream sync_module_terminal;
    pthread_t folder_watcher_thread; 
    
    queue<FileMetadata> modifications_queue;
    std::string root_folder_path;

}ClientStateInformation;




#endif  // __USER_TERMINAL_THREAD_INFO_HPP