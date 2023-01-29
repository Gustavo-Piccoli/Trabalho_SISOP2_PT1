#ifndef __DATAGRAM_STRUCTURES_HPP
#define __DATAGRAM_STRUCTURES_HPP

#include "userMeineBox.hpp"

typedef struct DatagramClientRequisition{
    UserMeineBox userMeineBox;  // For login and register
}DatagramClientRequisition;

#define DatagramServerOrder DatagramClientRequisition;

typedef struct DatagramData{

}DatagramData;


typedef struct DatagramSyncronizationStateList{
    
}DatagramSyncronizationStateList;



#endif // __DATAGRAM_STRUCTURES_HPP