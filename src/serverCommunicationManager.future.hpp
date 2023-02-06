#ifndef __COMMUNICATION_MANAGER_HPP
#define __COMMUNICATION_MANAGER_HPP

#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <pthread.h>

#include "global_definitions.hpp"
#include "ClientDeviceConected.hpp"

using namespace std;

void *serverSentinelModule(void *);
void *registerNewDataCommunicationSocket(void *);
void *runNewInfoDataCommunicationSocket(void *);
bool checkLogin(UserDataBag );

#endif