//
//  client.h
//  unix-communicator-client
//
//  Created by Maciej Żurad on 3/3/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#ifndef unix_communicator_client_client_h
#define unix_communicator_client_client_h

typedef void (*fnc_handler)(void*, int);

enum EXIT_CODES {SUCCESS, FAIL};

void* SHARED_MEM;

int EXIT_FLAG;
int LOGGED_IN;


int SERVER_CONNECTION;
int CLIENT_QUEUE;

char USERNAME[USER_NAME_MAX_LENGTH];

void init_client();
void init_client_resources();
void clean_exit();

void parseline(char* line);

void listener_loop(void* param);
void interface_loop(void* param);

void receive_msg(void* received, int msg_size, int msg_type, fnc_handler handler) ;

void handle_response(void* received, int msg_type);
void handle_message(void* received, int msg_type);
void handle_users_list(void* received, int msg_type);
void handle_rooms_list(void* received, int msg_type);

int perform_login();
void perform_logout();
void display_help();
void perform_room_action(int action_type, const char* roomname);

void send_private_message(const char* receiver, const char* content);

void display_room_list();
void display_user_list();

#endif
