#ifndef __USER_TERMINAL_FUNCTIONS_HPP
#define __USER_TERMINAL_FUNCTIONS_HPP

#include <iostream>
#include <unistd.h>
#include "userHelp.hpp"
#include "global_definitions.hpp"
#include "clientStateInformationStruct.hpp"
#include "DatagramStructures.hpp"
#include "SocketsOperation.hpp"

using namespace std;

void userTerminal_help(){
    cout << userCommandsHelpText << endl;
}

void userTerminal_login(ClientStateInformation *clientStateInformation){
    int info_data_communication_socket = clientStateInformation->info_data_communication_socket;
    ServerRequestResponseDatagram serverResponse;
    ClientRequestDatagram clientRequest;
    clientRequest.requisition_type = CLIENT_REQUEST_LOGIN;
    int bytes_number;

    // Login 
    cout << TERMINAL_TEXT_COLOR_WHITE;
    cout << "\t  >User Name: ";
    cout << TERMINAL_TEXT_COLOR_CYAN;
    cin >> clientRequest.userDataBag.login;
    cout << TERMINAL_TEXT_SETTING_RESET;
    cout << "\t  >Password: ";
    cout << TERMINAL_TEXT_COLOR_CYAN;
    // char ch;
    // clientRequest.userDataBag.passwd = "";
    // while(ch=cin.get()){
    //     cin.ignore();
    //     clientRequest.userDataBag.passwd += ch;
    //     cout << "*";
    // }
    cin >> clientRequest.userDataBag.passwd;
    cout << TERMINAL_TEXT_SETTING_RESET;
    
    // Request
    bytes_number = write(clientStateInformation->info_data_communication_socket, &clientRequest, REQUEST_DATAGRAM_SIZE);
    if(bytes_number == -1){
        cout << "\a    ## Couldn't contact server ..." << endl;
        return;
    }

    // Response
    bytes_number = read(clientStateInformation->info_data_communication_socket, &serverResponse, REQUEST_RESPONSE_DATAGRAM_SIZE );
    if(bytes_number == -1){
        cout << "\a    ## Couldn't receive answer from server ..." << endl;
        return;
    }

    if(serverResponse.was_login_validated_successfully){
        cout << TERMINAL_TEXT_COLOR_GREEN;
        cout << "    * Welcome back Mr. " << serverResponse.userDataBag.login << " !" << endl;
        cout << TERMINAL_TEXT_SETTING_RESET;
        clientStateInformation->is_user_logged = serverResponse.was_login_validated_successfully;
        clientStateInformation->userDataBag = serverResponse.userDataBag;
    }else{
        cout << TERMINAL_TEXT_COLOR_RED;
        cout << "\a    ## Invalid username or password! Couldn't login ..." << endl;
        cout << TERMINAL_TEXT_SETTING_RESET;
    }

}

void userTerminal_register(ClientStateInformation *clientStateInformation){

}

void userTerminal_start(ClientStateInformation *clientStateInformation){
    ClientRequestDatagram clientRequest;
    ServerRequestResponseDatagram serverResponse;
    int bytes_number;

    clientRequest.requisition_type = CLIENT_REQUEST_START;
    
    // Request
    bytes_number = write(clientStateInformation->info_data_communication_socket, &clientRequest, REQUEST_DATAGRAM_SIZE);
    if(bytes_number == -1){
        cout << TERMINAL_TEXT_COLOR_RED;
        cout << "\a\t  ## Couldn't contact server via ";
        cout << TERMINAL_TEXT_COLOR_BLUE;
        cout << "INFO SOCKET";
        cout << TERMINAL_TEXT_COLOR_RED;
        cout << "!" << endl;
        cout << TERMINAL_TEXT_SETTING_RESET;
        return;
    }

    // Response
    bytes_number = socket_read(clientStateInformation->info_data_communication_socket, &serverResponse, REQUEST_RESPONSE_DATAGRAM_SIZE );

    if(bytes_number == -1){
        cout << TERMINAL_TEXT_COLOR_RED;
        cout << "\a\t  ## Couldn't receive server answer via ";
        cout << TERMINAL_TEXT_COLOR_BLUE;
        cout << "INFO SOCKET";
        cout << TERMINAL_TEXT_COLOR_RED;
        cout << "!" << endl;
        cout << TERMINAL_TEXT_SETTING_RESET;
        return;
    }else{
        if( serverResponse.service_activation_state ){
            if(serverResponse.service_activation_is_already_this){
                cout << TERMINAL_TEXT_COLOR_YELLOW;
                cout << "\t  ** Service was already";
                cout << TERMINAL_TEXT_COLOR_GREEN;
                cout << " actived";
                cout << TERMINAL_TEXT_COLOR_YELLOW;
                cout << "!" << endl;
                cout << TERMINAL_TEXT_SETTING_RESET;
            }else{
                clientStateInformation->is_syncronization_active = true;

                cout << TERMINAL_TEXT_COLOR_GREEN;
                cout << "\t  ** Sync activated successfully!" << endl;
                cout << TERMINAL_TEXT_SETTING_RESET;
            }
        }else{
            cout << TERMINAL_TEXT_COLOR_RED;
            cout << "\a\t  ## Error: Can't start service!" << endl;
            cout << TERMINAL_TEXT_SETTING_RESET;
        }
    }
}

void userTerminal_stop(ClientStateInformation *clientStateInformation){
    if( !(clientStateInformation->is_syncronization_active) )
        cout << "  Service is already paused!" << endl;
    else
        clientStateInformation->is_syncronization_active = false;
}

void userTerminal_status(ClientStateInformation *clientStateInformation){
    if(clientStateInformation->is_user_logged){
        cout << "  > User logged: " << clientStateInformation->userDataBag.login << endl;
            cout << "  * Synchronize Service " << endl;
        if(clientStateInformation->is_syncronization_active){
            cout << TERMINAL_TEXT_COLOR_GREEN;
            cout << "ACTIVED";
            cout << TERMINAL_TEXT_SETTING_RESET;
        }else{
            cout << TERMINAL_TEXT_COLOR_RED;
            cout << "INACTIVED" << endl;
            cout << TERMINAL_TEXT_SETTING_RESET;
        }
            cout << "!" << endl;
    }else{
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
    }
}

void userTerminal_terminals_clear(){
    string cmd = "clear > /dev/pts/";

    // Clear each terminal
    for (char i = '1' ; i <= '6' ; i++)
        system((cmd + i).c_str());
}

void userTerminal_terminal_clear(int terminal_number){
    string cmd = "clear > /dev/pts/";
    cmd += (char)('0'+terminal_number );

    // Clear terminal
    system( cmd.c_str() );
}

void userTerminal_terminals_info(){
    string cmd1 = "tty > /dev/pts/";
    string cmd2 = " > /dev/pts/";
    string info[] = {
        "============================== DataBag SERVER: SENTINEL THREAD ======================",
        "============================== DB WATCHER Module ==============================",
        "============================== SERVER SYNC ==============================",
        "============================== Client Terminal ==============================",
        "============================== Client Folder WATCHER Module ==========================",
        "============================== Client SYNC Module =========================="
    };

    for (char i = '1' ; i <= '6' ; i++){
        system((cmd1 + i).c_str());
        system((info[i-'0'] + cmd2 + i).c_str()); 
    }
}





#endif // __USER_TERMINAL_FUNCTIONS_HPP