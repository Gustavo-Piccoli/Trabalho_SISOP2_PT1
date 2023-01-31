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

}ClientRequestDatagram;

typedef ClientRequestDatagram ServerRequestResponseDatagram;

typedef struct DataDatagram{

}DataDatagram;

typedef struct SyncListDatagram{

}SyncListDatagram;

#endif // __DATAGRAM_STRUCTURES_HPP