#ifndef __FILE_MANAGER_HPP
#define __FILE_MANAGER_HPP

#include <iostream>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <string.h>

#include "DatagramStructures.hpp"

using namespace std;

void fill_sync_list(char* path, SyncList* sync_list){
    DIR *dir;
    struct dirent *ent;
    struct stat st;
    if ((dir = opendir (path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if(ent->d_name[0] != '.'){
                char file_path[512];
                sprintf(file_path, "%s/%s", path, ent->d_name);
                if(stat(file_path, &st) == 0 && ent->d_type != DT_DIR){
                    file_info f;
                    memset(f.name, 0, MAX_FILE_NAME_SIZE);
                    strncpy(f.name, file_path, MAX_FILE_NAME_SIZE);
                    f.last_modification = st.st_mtime;
                    sync_list->files[sync_list->num_files++] = f;
                }
                if(ent->d_type == DT_DIR){
                    fill_sync_list(file_path, sync_list);
                }
            }
        }
        closedir(dir);
    }
}

int* compare_sync_lists(SyncList* client_sync_list, SyncList* server_sync_list){
    int* result_list = new int[std::max(client_sync_list->num_files, server_sync_list->num_files)];
    for(int i = 0; i < client_sync_list->num_files; i++){
        result_list[i] = -1;
        for(int j = 0; j < server_sync_list->num_files; j++){
            if(strcmp(client_sync_list->files[i].name, server_sync_list->files[j].name) == 0 ){
                if(server_sync_list->files[j].last_modification > client_sync_list->files[i].last_modification){
                    result_list[i] = j;
                } 
            }
        }
    }
    
    if(client_sync_list->num_files < server_sync_list->num_files )
        for( int i = client_sync_list->num_files ; i < server_sync_list->num_files ; i++ )
            result_list[i] = i;
    return result_list;
}

#endif  //__FILE_MANAGER_HPP