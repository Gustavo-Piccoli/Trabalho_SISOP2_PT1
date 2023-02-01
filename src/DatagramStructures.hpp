#ifndef __DATAGRAM_STRUCTURES_HPP
#define __DATAGRAM_STRUCTURES_HPP

#include "userMeineBox.hpp"

typedef struct ClientRequestDatagram{
    int requisition_type;
    UserMeineBox userMeineBox;  // For login and register
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
    bool should_receive_file;
    bool should_send_file;
    char filename[MAX_FILE_NAME_SIZE];

}DataDatagram;

struct file_info{
    char name[MAX_FILE_NAME_SIZE];
    time_t last_modification;
};

struct SyncList{
    struct file_info files[MAX_SYNC_LIST_SIZE];
    int num_files;
};

#endif // __DATAGRAM_STRUCTURES_HPP