#ifndef __USER_TERMINAL_THREAD_INFO_HPP
#define __USER_TERMINAL_THREAD_INFO_HPP

#include "userMeineBox.hpp"

typedef struct ClientStateInformation{
    int info_data_communication_socket;
    int sync_data_communication_socket;
    bool is_syncronization_active;
    bool is_user_logged;
    bool is_connectded;
    UserMeineBox userMeineBox;
}ClientStateInformation;




#endif  // __USER_TERMINAL_THREAD_INFO_HPP