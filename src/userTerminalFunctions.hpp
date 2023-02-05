#ifndef __USER_TERMINAL_FUNCTIONS_HPP
#define __USER_TERMINAL_FUNCTIONS_HPP

#include <iostream>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "userHelp.hpp"
#include "userMeineBox.hpp"
#include "global_definitions.hpp"
#include "clientStateInformationStruct.hpp"
#include "DatagramStructures.hpp"
#include <sys/socket.h>

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
    cout << "\t  >User Name: ";
    cin >> clientRequest.userMeineBox.login;

    cout << "\t  >Password: " << endl;
    cin >> clientRequest.userMeineBox.passwd;
    
    // Request
    bytes_number = write(clientStateInformation->info_data_communication_socket, &clientRequest, REQUEST_DATAGRAM_SIZE);
    if(bytes_number == -1){
        cout << "  ## Couldn't contact server ..." << endl;
        return;
    }

    // Response
    bytes_number = read(clientStateInformation->info_data_communication_socket, &serverResponse, REQUEST_RESPONSE_DATAGRAM_SIZE );
    if(bytes_number == -1){
        cout << "  ## Couldn't contact server ..." << endl;
        return;
    }

    if(serverResponse.was_login_validated_successfully){
        cout << "  >> Welcome back Mr. " << serverResponse.userMeineBox.login << " !" << endl;
        clientStateInformation->is_user_logged = serverResponse.was_login_validated_successfully;
        clientStateInformation->userMeineBox = serverResponse.userMeineBox;
        //return clientStateInformation->userMeineBox.login;
    }else{
        cout << "  ## Invalid username or password! Couldn't login ..." << endl;
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
        cout << "  ## Couldn't contact server ..." << endl;
        return;
    }

    // Response
    bytes_number = read(clientStateInformation->info_data_communication_socket, &serverResponse, REQUEST_RESPONSE_DATAGRAM_SIZE );
    if(bytes_number == -1){
        cout << "  ## Couldn't contact server ..." << endl;
        return;
    }
    
    if( serverResponse.service_activation_is_already_this )
        cout << "  Service is already active!" << endl;
    else{
        if( serverResponse.service_activation_state )
            clientStateInformation->is_syncronization_active = true;
        else
            cout << "\a  ## Error: Can't start service!" << endl;
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
        cout << "  > User logged: " << clientStateInformation->userMeineBox.login << endl;
        if(clientStateInformation->is_syncronization_active)
            cout << "  > Active Service!" << endl;
        else
            cout << "  > Inactive Service!" << endl;
    }else{
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
    }
}

void userTerminal_download(ClientStateInformation *ClientStateInformation, string command)
{
    if (ClientStateInformation->is_user_logged)
    {
        std::string s = command;
        std::string delimiter = "_";
        std::string filename = s.substr(s.find(delimiter) + 1, s.length()); // get the filename
    }
    else
    {
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
    }

}

void userTerminal_upload(ClientStateInformation *ClientStateInformation, string command){
    if(ClientStateInformation->is_user_logged){
        std::string s = command;
        std::string delimiter = "_";
        std::string filename = s.substr(s.find(delimiter) + 1,s.length()); // get the filename
        const char* str_filename = filename.c_str();
        FILE *file = fopen(str_filename, "rw");
        if(!file){
            cout<<"This file doesn´t exist"<<endl;
        }else{
            send(ClientStateInformation->info_data_communication_socket, file, sizeof(file),0); //send the file to the server

            char new_filename [200];
            strcpy(new_filename,"./server_folder");
            mkdir(new_filename, S_IRUSR | S_IWUSR | S_IXUSR);
            strcat(new_filename,"/");
            strcat(new_filename,str_filename);
        


        }
    }else{
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
    }

}

void userTerminal_delete(ClientStateInformation *ClientStateInformation, string command){
    if(ClientStateInformation->is_user_logged){

        std::string s = command;
        std::string delimiter = "_";
        std::string filename= s.substr(s.find(delimiter) + 1,s.length()); // get the filename

    }else{
        cout << "  No one logged in!" << endl;
        cout << "  (Use 'login' command or type 'help' for more explanation ...)" << endl;
    }

}



#endif // __USER_TERMINAL_FUNCTIONS_HPP