#ifndef __GLOBAL_DEFINITIONS_HPP
#define __GLOBAL_DEFINITIONS_HPP

// MeineBox
#define MEINEBOX_VERSION 0.1

// OS ENVIROMENT 
#define SYNCRONIZE_FOLDER "~/sync_dir" 

// SOCKETS
#define SENTINEL_SOCKET_PORT 7779
#define SENTINEL_SOCKET_QUEUE_CAPACITY 20
#define CLIENT_RECEIVE_CONNECTION_PORT 8841
#define SYNC_SOCKET_QUEUE_CAPACITY 10
#define DATA_COMMUNICATION_BUFFER_CAPACITY 1024

// DATAGRAM COMMUNICATION
#define REQUEST_DATAGRAM_SIZE sizeof(ClientRequestDatagram)
#define REQUEST_RESPONSE_DATAGRAM_SIZE sizeof(ServerRequestResponseDatagram)

#define CLIENT_REQUEST_QUIT                     0x0000
#define CLIENT_REQUEST_LOGIN                    0x0001
#define CLIENT_REQUEST_REGISTER                 0x0002
#define CLIENT_REQUEST_START                    0x0003
#define CLIENT_REQUEST_STATUS                   0x0004
#define CLIENT_REQUEST_STOP                     0x0005
#define CLIENT_DATAGRAM_SYNC_STATE_LIST         0x0006

#define SERVER_SYNC_ORDER_RECEIVE_DATAGRAM      0x0007
#define SERVER_SYNC_ORDER_SEND_DATAGRAM         0x0008

#define SYNC_DATAGRAM                           0x0009

#define CLIENT_REQUEST_UPLOAD                   0x0010

// TERMINAL COLOURS
#define TERMINAL_TEXT_COLOR_BLACK "\033[1;30m"
#define TERMINAL_TEXT_COLOR_RED "\033[1;31m"
#define TERMINAL_TEXT_COLOR_GREEN "\033[1;32m"
#define TERMINAL_TEXT_COLOR_YELLOW "\033[1;33m"
#define TERMINAL_TEXT_COLOR_BLUE "\033[1;34m"
#define TERMINAL_TEXT_COLOR_MAGENTA "\033[1;35m"
#define TERMINAL_TEXT_COLOR_CYAN "\033[1;36m"
#define TERMINAL_TEXT_COLOR_WHITE "\033[1;37m"
#define TERMINAL_TEXT_SETTING_RESET "\033[1;0m"

// Sync List
#define MAX_FILE_NAME_SIZE 150
#define MAX_SYNC_LIST_SIZE 2000

#endif