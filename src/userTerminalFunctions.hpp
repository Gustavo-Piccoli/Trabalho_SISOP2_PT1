#ifndef __USER_TERMINAL_FUNCTIONS_HPP
#define __USER_TERMINAL_FUNCTIONS_HPP

#include "userHelp.hpp"

void userTerminal_help(){
      cout << userCommandsHelpText << endl;
}

void userTerminal_login(int data_communication_socket){
      // Login 
      char data_buffer[DATA_COMMUNICATION_BUFFER_CAPACITY] = "";
      int bytes_read;

      bytes_read = read(data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);
      cout << data_buffer;
      cin >> data_buffer;
      write(data_communication_socket, &data_buffer, DATA_COMMUNICATION_BUFFER_CAPACITY);
}

void userTerminal_register(int data_communication_socket){

}

void userTerminal_start(bool *is_syncronization_active){
      if( *is_syncronization_active )
        cout << "  Service is already active!" << endl;
      else
        *is_syncronization_active = true;
}

void userTerminal_stop(bool *is_syncronization_active){
      if( !(*is_syncronization_active) )
        cout << "  Service is already paused!" << endl;
      else
        *is_syncronization_active = false;
}

void userTerminal_status(bool *is_syncronization_active){
      if(*is_syncronization_active){
        cout << "  Active Service!" << endl;
      }else
        cout << "  Inactive Service!" << endl;
}





#endif // __USER_TERMINAL_FUNCTIONS_HPP