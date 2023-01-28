#ifndef __CLIENT_DEVICE_CONECTED_HPP
#define __CLIENT_DEVICE_CONECTED_HPP

#include <netinet/in.h>
#include "userMeineBox.hpp"

typedef struct ClientDeviceConected{
    int socket_fd;
    sockaddr_in client_device_address;
    UserMeineBox userMeineBox;
    bool login_validated;
}ClientDeviceConected;

#endif