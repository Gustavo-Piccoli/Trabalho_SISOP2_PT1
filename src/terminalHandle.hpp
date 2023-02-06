#ifndef __TERMINAL_HANDLE_HPP
#define __TERMINAL_HANDLE_HPP

#include <iostream>

using namespace std;

void clearServerTerminal(string);

void clearServerTerminal(string terminal){
    string cmd = " clear > " + terminal;
    system(cmd.c_str()); 
}

#endif // __TERMINAL_HANDLE_HPP