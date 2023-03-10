#ifndef __USER_HELP_HPP
#define __USER_HELP_HPP


#define userCommandsHelpText "\
  COMMAND           |          DESCRIPTION                                                   \n\
  help              |  Show this informative table                                           \n\
  login             |  Make login in the server                                              \n\
  quit              |  Close MeineBox program and stop syncronization service                \n\
  register          |  Register new user in the server(sign in)                              \n\
  start             |  After logged, this command initialize the sync service                \n\
  status            |  Show actual user logged and syncronization service status             \n\
  stop              |  Stop the sync service(if it is running)                               \n\
  upload filename   |  Upload the file to the users sync_dir folder                          \n\
  download filename |  Download the file to the users sync_dir folder                        \n\
  delete filename   |  Delete the file from the users sync_dir folder                        \n\
"

#endif