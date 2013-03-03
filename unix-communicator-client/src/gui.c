//
//  gui.c
//  unix-communicator-client
//
//  Created by Maciej Żurad on 1/26/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#include <stdio.h>

#include "gui.h"

WINDOW *cmd_win = NULL;
WINDOW *chat_win = NULL;
WINDOW *user_win = NULL;

int max_cols;
int max_rows;

/**
 Creates window and returns handler to it
 @param height windows's height
 @param width  windows's width
 @param starting_y starting point of a window on y axis
 @param starting_x starting point of a window on x axis
 @returns handler of newly created window
 */
WINDOW *create_newwin(int height, int width, int starting_y, int starting_x){
    WINDOW *new_win = newwin(height, width, starting_y, starting_x);
    box(new_win, 0, 0);
    scrollok(new_win, TRUE);
    wrefresh(new_win);
    return new_win;
}

/**
 Destroys window
 @param win handler
 */
void destroy_win(WINDOW *win){
    wborder(win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
    wrefresh(win);
    delwin(win);
}

/**
 Grabs current terminal width and height in lines and cols
 saves result through ptrs
 @param x width
 @param y height
 */
void getsizes(int* x, int *y){
    struct winsize ws;
    ioctl(fileno(stdin), TIOCGWINSZ, &ws);
    *x = ws.ws_col;
    *y = ws.ws_row;
}

/**
 Creates GUI with proper aspect ratios
 */
void create_gui(){
    getsizes(&max_cols, &max_rows);
    
    chat_win = create_newwin(max_rows - 7, max_cols - 16, 0, 0);
    cmd_win = create_newwin(7, max_cols, max_rows - 7, 0);
    user_win = create_newwin(max_rows - 7, 15, 0, max_cols - 15);
    
    keypad(chat_win, TRUE);
    keypad(cmd_win, TRUE);
    keypad(user_win, TRUE);
    
    refresh_gui();
    refresh();
}


/**
 Redraws terminal window with proper aspect ratios
 after catching SIGWINCH signal
 */
void resizehandler(){
    getsizes(&max_cols, &max_rows);

    resize_term(max_rows, max_cols);
    clear();    
    destroy_win(chat_win);
    destroy_win(cmd_win);
    destroy_win(user_win);
    chat_win = create_newwin(max_rows - 7, max_cols - 16, 0, 0);
    cmd_win = create_newwin(7, max_cols, max_rows - 7, 0);
    user_win = create_newwin(max_rows - 7, 15, 0, max_cols - 15);
    refresh_gui();
}

/**
 Refreshes GUI
 */
void refresh_gui(){
    wrefresh(chat_win);
    wrefresh(cmd_win);
    wrefresh(user_win);
}

/**
 Initializes GUI
 */
void init_gui(){
    printf("Initializing GUI.... \n\n");
    initscr();
    clear();
    if(!has_colors()){
        endwin();
        printf("Your terminal does not support colours\n");
        exit(EXIT_FAILURE);
    }
    start_color();
    signal(SIGWINCH, resizehandler);
}

/**
 Closes GUI
 */
void close_gui(){
    destroy_win(cmd_win);
    destroy_win(chat_win);
    destroy_win(user_win);
    endwin();
    printf("Closing GUI.... \n\n");
}