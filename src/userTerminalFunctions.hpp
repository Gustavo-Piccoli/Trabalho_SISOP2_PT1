#ifndef __USER_TERMINAL_FUNCTIONS_HPP
#define __USER_TERMINAL_FUNCTIONS_HPP

#include <iostream>
#include <unistd.h>
#include "userHelp.hpp"
#include "global_definitions.hpp"
#include "clientStateInformationStruct.hpp"

using namespace std;

void userTerminal_help(){
    cout << userCommandsHelpText << endl;
}

void userTerminal_login(ClientStateInformation *clientStateInformation){
    int info_data_communication_socket = clientStateInformation->info_data_communication_socket;
    char data_buffer[DATA_COMMUNICATION_BUFFER_CAPACITY] = "";
    int bytes_read;

    // Login 
    bytes_read = read(info_data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);
    cout << data_buffer;
    cin >> data_buffer;
    write(info_data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);

    // Password 
    bytes_read = read(info_data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);
    cout << data_buffer;
    cin >> data_buffer;
    write(info_data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);

    // Verification
    bytes_read = read(info_data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);
    if(1){
        cout << "Welcome Mr. " << endl;
    }
}

void userTerminal_register(ClientStateInformation *clientStateInformation){

}

void userTerminal_start(ClientStateInformation *clientStateInformation){
    
    if( clientStateInformation->is_syncronization_active )
        cout << "  Service is already active!" << endl;
    else
        clientStateInformation->is_syncronization_active = true;
}

void userTerminal_stop(ClientStateInformation *clientStateInformation){
    if( !(clientStateInformation->is_syncronization_active) )
        cout << "  Service is already paused!" << endl;
    else
        clientStateInformation->is_syncronization_active = false;
}

void userTerminal_status(ClientStateInformation *clientStateInformation){
    if(clientStateInformation->is_user_logged){
        cout << "  > User logged: " << clientStateInformation->userMeineBox.login << endl;
        if(clientStateInformation->is_syncronization_active)
            cout << "  > Active Service!" << endl;
        else
            cout << "  > Inactive Service!" << endl;
    }else
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
}





#endif // __USER_TERMINAL_FUNCTIONS_HPP