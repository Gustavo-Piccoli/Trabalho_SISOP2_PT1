#ifndef __FILE_MANAGER_HPP
#define __FILE_MANAGER_HPP

#include <iostream>
#include <algorithm>
#include <list>
#include <iterator>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <string.h>
#include <filesystem>
#include <sys/inotify.h>

#include "DatagramStructures.hpp"
#include "ClientDeviceConected.hpp"
#include "clientStateInformationStruct.hpp"
#include "global_definitions.hpp"

using namespace std;

std::list<string> avoid_files {
    ".goutputstream-",
    ".goutputstream-"
};



void *server_folder_watcher_module(void *clientDeviceConnected_arg) {
    ClientDeviceConnected *clientDeviceConnected = (ClientDeviceConnected*) clientDeviceConnected_arg;
    ofstream tty(TERMINAL_SERVER_DB_WATCHER, ofstream::out | ofstream::app );

    // Initializing modified files queue
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << "  ** Modified files queue initialized ..." << endl;

    // Initialize inotify instance
    int fd = inotify_init();

    // Add a watch for the folder
    int watch = inotify_add_watch(fd, clientDeviceConnected->db_folder_path.c_str(), IN_ALL_EVENTS);

    // Create a buffer for events
    const int BUF_LEN = EVENTS_INOTIFY_BUFFER_LEN;
    char buffer[BUF_LEN];

    // Read events from inotify
    int len = read(fd, buffer, BUF_LEN);

    // Parse the events
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << "  ** DB: Server Folder Watcher Module initialized: ";
    tty << TERMINAL_TEXT_COLOR_CYAN;
    tty << clientDeviceConnected->userDataBag.login << endl;
    tty << TERMINAL_TEXT_SETTING_RESET;
    tty << "  \t> Path: ";
    tty << TERMINAL_TEXT_COLOR_CYAN;
    tty << clientDeviceConnected->db_folder_path << endl;
    tty << TERMINAL_TEXT_SETTING_RESET;


    while (clientDeviceConnected->is_connected) {
        int i = 0;
        while (i < len) {
            inotify_event *event = (inotify_event *) &buffer[i];
            if (event->len > 0) {
                // Do nothing if is a avoid file
                string event_name = event->name;
                bool avoid_this_file = false;
                for( list<string>::iterator it = avoid_files.begin() ; it != avoid_files.end() ; ++it){
                    if( event_name.find(*it) != string::npos )
                        avoid_this_file = true;
                }
                if( !avoid_this_file ){
                    // Getting event emiter file informations
                    struct stat file_info;
                    FileMetadata fileMetadata;
                    
                    stat((clientDeviceConnected->db_folder_path + "/" + event->name).c_str(), &file_info);
                    strcpy(fileMetadata.name, event->name);
                    fileMetadata.size = file_info.st_size;
                    fileMetadata.up_to_date_with_server = false;

                    // Print the name of the file that was modified, created, or deleted
                    string action;
                    if (event->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
                        action = "modified";
                    }else if(event->mask & (IN_CREATE)){
                        action = "created";
                    }else if(event->mask & (IN_MOVED_TO)){
                        action = "moved to DB";
                    }else if(event->mask & (IN_MOVED_TO)){
                        action = "moved from DB";
                    }else if(event->mask & (IN_DELETE))
                        action = "deleted";
                    action = " " + action;

                    if (event->mask & (IN_MODIFY | IN_CLOSE_WRITE | IN_CREATE | IN_MOVED_TO) ) {
                        fileMetadata.should_delete_file = false;
                        clientDeviceConnected->modified_files_queue.push(fileMetadata);
                        
                        tty << TERMINAL_TEXT_COLOR_GREEN;
                        tty << "  ++" << clientDeviceConnected->userDataBag.login << ": " << event->name << action << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET; 
                    } else if (event->mask & (IN_DELETE | IN_MOVED_FROM)) {
                        fileMetadata.should_delete_file = true;
                        clientDeviceConnected->modified_files_queue.push(fileMetadata);

                        tty << TERMINAL_TEXT_COLOR_YELLOW;
                        tty << "--" << clientDeviceConnected->userDataBag.login << ": " << event->name << action  << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET; 
                    }
                }
            }
            i += sizeof(inotify_event) + event->len;
        }
        len = read(fd, buffer, BUF_LEN);
    }

    // Remove the watch
    inotify_rm_watch(fd, watch);

    // Close the inotify instance
    close(fd);

    return NULL;
}

void *client_folder_watcher_module(void *clientStateInformation_arg) {
    ClientStateInformation *clientStateInformation = (ClientStateInformation*) clientStateInformation_arg;
    string folderPath = SYNCRONIZE_FOLDER;
    ofstream tty(TERMINAL_CLIENT_FOLDER_WATCHER, ofstream::out | ofstream::app);
   
    tty << TERMINAL_TEXT_COLOR_WHITE;
    tty << "  ** Client Folder Watcher Module initialized ..." << endl;
    tty << TERMINAL_TEXT_SETTING_RESET;

    // Fix HOME folder name
    string home_dir = getenv("HOME");
    home_dir += '/';
    string home_sign = "~/";
    auto last_ocur = 0;
    while((last_ocur = folderPath.find(home_sign,last_ocur)) != string::npos){
        folderPath.replace( last_ocur, home_sign.length(), home_dir );
        last_ocur += home_dir.length();
    }
    clientStateInformation->root_folder_path = folderPath + "/";
   
    // Cleaning sync dir folder
    // Check if the folder is empty
    bool empty = true;
    for (const auto& entry : std::filesystem::directory_iterator(clientStateInformation->root_folder_path)) {
        empty = false;
        break;
    }
    if(!empty){
        string cleaning_sync_dir_cmd = "rm " + clientStateInformation->root_folder_path + "*";
        system(cleaning_sync_dir_cmd.c_str());
    }

    tty << TERMINAL_TEXT_COLOR_WHITE;
    tty << endl << "  ** Synchronization module prepared ..." << endl;
    tty << TERMINAL_TEXT_SETTING_RESET;


    tty << TERMINAL_TEXT_COLOR_WHITE;
    tty << "  ** Folder synchronized: ";
    tty << TERMINAL_TEXT_COLOR_CYAN;
    tty << clientStateInformation->root_folder_path << endl;
    tty << TERMINAL_TEXT_SETTING_RESET;

    // Initialize inotify instance
    int fd = inotify_init();

    // Add a watch for the folder
    int watch = inotify_add_watch(fd, folderPath.c_str(), IN_ALL_EVENTS);

    // Create a buffer for events
    const int BUF_LEN = EVENTS_INOTIFY_BUFFER_LEN;
    char *buffer = new char[BUF_LEN];
    // Read events from inotify
    int len = read(fd, buffer, BUF_LEN);

    // Parse the events
    while (clientStateInformation->is_connected) {
        int i = 0;
        while (i < len) {
            inotify_event *event = (inotify_event *) &buffer[i];
            if (event->len > 0) { 
                // Do nothing if is a avoid file
                string event_name = event->name;
                bool avoid_this_file = false;
                for( list<string>::iterator it = avoid_files.begin() ; it != avoid_files.end() ; ++it){
                    if( event_name.find(*it) != string::npos )
                        avoid_this_file = true;
                }
                if( !avoid_this_file ){
                    // Print the name of the file that was modified, created, or deleted
                    string action;
                    if (event->mask & (IN_MODIFY | IN_CLOSE_WRITE)) {
                        action = "modified";
                    }else if(event->mask & (IN_CREATE)){
                        action = "created";
                    }else if(event->mask & (IN_MOVED_TO)){
                        action = "moved to this folder";
                    }else if(event->mask & (IN_MOVED_FROM)){
                        action = "moved from this folder";
                    }else if(event->mask & (IN_DELETE))
                        action = "deleted";
                    action = " " + action;

                    // Getting event emiter file informations
                    FileMetadata fileMetadata;
                    struct stat file_info;

                    stat( (clientStateInformation->root_folder_path + event->name).c_str(), &file_info);
                    strcpy(fileMetadata.name, event->name);
                    fileMetadata.size = file_info.st_size;
                    fileMetadata.up_to_date_with_client = false;

                    // Print the name of the file that was modified, created, or deleted
                    //  Add modified file to modified queue. If file is deleted, flag should_delete is active
                    if (event->mask & (IN_CLOSE_WRITE | IN_MODIFY | IN_CREATE | IN_MOVED_TO)) {
                        if( event->name)
                        fileMetadata.should_delete_file = false;
                        // clientStateInformation->modifications_queue.push(fileMetadata);
                        
                        tty << TERMINAL_TEXT_COLOR_GREEN;
                        tty << "  ++" << clientStateInformation->userDataBag.login << ": " << event->name << action << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET; 
                    } else if (event->mask & (IN_DELETE | IN_DELETE_SELF | IN_MOVED_FROM)) {
                        fileMetadata.should_delete_file = true;
                        // clientStateInformation->modifications_queue.push(fileMetadata);

                        tty << TERMINAL_TEXT_COLOR_YELLOW;
                        tty << "  --" << clientStateInformation->userDataBag.login << ": " << event->name << action << endl;
                        tty << TERMINAL_TEXT_SETTING_RESET;
                    }
                }
            }
            i += sizeof(inotify_event) + event->len;
        }
        len = read(fd, buffer, BUF_LEN);
    }

    // Remove the watch
    inotify_rm_watch(fd, watch);

    // Close the inotify instance
    close(fd);
    delete[] buffer;

    return NULL;
}
























// void fill_sync_list(char* path, SyncList* sync_list){
//     DIR *dir;
//     struct dirent *ent;
//     struct stat st;
//     if ((dir = opendir (path)) != NULL) {
//         while ((ent = readdir (dir)) != NULL) {
//             if(ent->d_name[0] != '.'){
//                 char file_path[512];
//                 sprintf(file_path, "%s/%s", path, ent->d_name);
//                 if(stat(file_path, &st) == 0 && ent->d_type != DT_DIR){
//                     FileMetadata f;
//                     memset(f.name, 0, MAX_FILE_NAME_SIZE);
//                     strncpy(f.name, file_path, MAX_FILE_NAME_SIZE);
//                     f.last_modification = st.st_mtime;
//                     sync_list->files[sync_list->num_files++] = f;
//                 }
//                 if(ent->d_type == DT_DIR){
//                     fill_sync_list(file_path, sync_list);
//                 }
//             }
//         }
//         closedir(dir);
//     }
// }

// int* compare_sync_lists(SyncList* client_sync_list, SyncList* server_sync_list){
//     int* result_list = new int[std::max(client_sync_list->num_files, server_sync_list->num_files)];
//     for(int i = 0; i < client_sync_list->num_files; i++){
//         result_list[i] = -1;
//         for(int j = 0; j < server_sync_list->num_files; j++){
//             if(strcmp(client_sync_list->files[i].name, server_sync_list->files[j].name) == 0 ){
//                 if(server_sync_list->files[j].last_modification > client_sync_list->files[i].last_modification){
//                     result_list[i] = j;
//                 } 
//             }
//         }
//     }
    
//     if(client_sync_list->num_files < server_sync_list->num_files )
//         for( int i = client_sync_list->num_files ; i < server_sync_list->num_files ; i++ )
//             result_list[i] = i;
//     return result_list;
// }

#endif  //__FILE_MANAGER_HPP