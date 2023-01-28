#ifndef __USER_TERMINAL_THREAD_INFO_HPP
#define __USER_TERMINAL_THREAD_INFO_HPP

#include "userMeineBox.hpp"

typedef struct UserTerminalThreadInfo{
    int data_communication_socket;
    bool is_syncronization_active;
}UserTerminalThreadInfo;




#endif  // __USER_TERMINAL_THREAD_INFO_HPP