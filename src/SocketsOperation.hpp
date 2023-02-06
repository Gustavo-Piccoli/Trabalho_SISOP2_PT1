#ifndef __SOCKETS_OPERATIONS_HPP
#define __SOCKETS_OPERATIONS_HPP


template <typename T>
int socket_read(int socket_fd, T* buffer, int size){
    int     dataRead = 0;
    int     dataMax  = size - 1;

    while(dataRead < dataMax){
        int get = read(socket_fd, buffer + dataRead, size - dataRead);
        if (get == -1)
            return -1;
        if (get == 0)
            break;
        dataRead += get;
    }
    // buffer[dataRead] = '\0';
    
    return dataRead;
}



#endif // __SOCKETS_OPERATIONS_HPP