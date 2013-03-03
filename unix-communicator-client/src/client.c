//
//  main.c
//  unix-communicator-client
//
//  Created by Maciej Żurad on 1/26/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"
#include "client.h"

#define cmp(line, text, nr, letter) !(strncmp(line, text, nr) && strncmp(line, letter, 2)) 

int master_pid;
int listener_pid;


int main(int argc, char *argv[])
{
    master_pid = getpid();

    
    size_t restrict_size = 512;
    char* line = calloc(restrict_size, sizeof(char));
    
    if( !(listener_pid = fork()) ) {
        
    } else {
        while(1){
            getline(&line, &restrict_size, stdin);
            parseline(line);
        }
    }
    
    
    
    return 0;
}


       
void parseline(char* line){
    if ( cmp(line, "/quit", 5, "/q") ) {
       
       printf("quitting\n");
       
    } else if ( cmp(line, "/join ", 6, "/j") ) {
       
       printf("joining channel %s \n", line);
       
    } else if ( cmp(line, "/leave", 5, "/l") ) {
       
       printf("leaving channel\n");
       
    } else if ( cmp(line, "/msg ", 5, "/w") ) {
       
       printf("messaging %s\n", line);
       
    } else if ( cmp(line, "/help", 5, "/h") ) {
       
       printf("showing help\n");
       
    } else if ( cmp(line, "/rooms", 6, "/r") ) {
       
       printf("showing rooms\n");
       
    } else if ( cmp(line, "/users", 6, "/u") ) {
       
       printf("showing users\n");
       
    }
}