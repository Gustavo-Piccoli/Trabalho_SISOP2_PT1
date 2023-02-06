#ifndef __DATAGRAM_STRUCTURES_HPP
#define __DATAGRAM_STRUCTURES_HPP

#include <sys/stat.h>
#include "userDataBag.hpp"

typedef struct ClientRequestDatagram{
    int requisition_type;
    UserDataBag userDataBag;  // For login and register
    bool service_activation_state; // For start and stop 
    bool client_conection_state; // For quit 

    // Server Reponse Exclusive Use
    bool was_a_valid_user_name;  // For register
    bool was_a_valid_pasword;   // For register
    bool was_login_validated_successfully; // For login
    bool service_activation_is_already_this; // For start and stop 

}ClientRequestDatagram;

typedef ClientRequestDatagram ServerRequestResponseDatagram;

typedef struct DataDatagram{
    bool client_should_receive_file;
    bool client_should_send_file;
    char filename[MAX_FILE_NAME_SIZE];
    char *buffer;
}DataDatagram;

struct FileMetadata{
    char name[MAX_FILE_NAME_SIZE];
    int size;
    
    bool should_delete_file;
    bool up_to_date_with_server;
    bool up_to_date_with_client;
};

struct SyncList{
    struct FileMetadata files[MAX_SYNC_LIST_SIZE];
    int num_files;
};

const string REQUESTS_NAMES[]{
    "CLIENT_REQUEST_QUIT",    
    "CLIENT_REQUEST_LOGIN",   
    "CLIENT_REQUEST_REGISTER",
    "CLIENT_REQUEST_START",   
    "CLIENT_REQUEST_STATUS",  
    "CLIENT_REQUEST_STOP"    
};

#endif // __DATAGRAM_STRUCTURES_HPP