//
//  io.c
//  unix-communicator-client
//
//  Created by Maciej Żurad on 1/26/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>

#define MAX_MSG_LENGTH 256
#include "io.h"

char **chat_buffer = NULL;

/**
 Allocates memory for chat buffer
 @param nr_lines number of lines for chat buffer
 @param max_length max length of single line
 @returns returns pointer to chat buffer
 */
char** allocate_mem(int nr_lines, int max_length){
    char **temp = (char**)malloc(nr_lines * sizeof(char*));
    int i;
    for(i=0; i<nr_lines; ++i){
        temp[i] = (char*)malloc((max_length+1)*sizeof(char));
    }
    return temp;
}

/**
 Reads line from command/input/msg window
 @param win handler to window that reading takes place in
 @param text pointer to buffer that will contain read line
 */
void readline(WINDOW *win, char* text){
    char buf;
    short pos_x = 0, pos_y = max_rows-2;
    while ((buf = mvwgetch(win, pos_y, pos_x)) != '\n'){
        text[pos_x] = buf;
        pos_x++;
    }
    text[pos_x] = '\0';
}
