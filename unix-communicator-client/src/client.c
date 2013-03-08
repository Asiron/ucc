//
//  main.c
//  unix-communicator-client
//
//  Created by Maciej Żurad on 1/26/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#include "protocol.h"
#include "client.h"

#define cmp(line, text, nr) !strncmp(line, text, nr)




void init_client(){

    EXIT_FLAG = FALSE;
    LOGGED_IN = FALSE;
    
    CLIENT_QUEUE = msgget(IPC_PRIVATE, 0666);
    
    strcpy(CHANNEL, "");
    
    signal(SIGINT, clean_exit);
}

void clean_exit(){
    printf("\n%sFreeing up resources\n", KBLU);
    printf("%sQuitting\n", KBLU);
    msgctl(CLIENT_QUEUE, IPC_RMID, 0);
    signal(SIGINT, SIG_DFL);
    EXIT_FLAG = TRUE;
}

int main(int argc, char *argv[])
{
    init_client();
    
    pthread_t listener_thread;
    pthread_t interface_thread;
    
    void (listener_loop)();
    void (interface_loop)();
    pthread_create(&interface_thread, NULL, (void*) &interface_loop, NULL);
    pthread_create(&listener_thread, NULL, (void*) &listener_loop, NULL);
    
    //block main until both threads exit
    pthread_join(listener_thread, NULL);
    pthread_join(interface_thread, NULL);
    
    return EXIT_SUCCESS;
}

void listener_loop(void* param){
    void* received = malloc(2048);
    while(!EXIT_FLAG){
        receive_msg(received, _size(MSG_RESPONSE), RESPONSE, &handle_response);
        receive_msg(received, _size(MSG_CHAT_MESSAGE), MESSAGE, &handle_message);
        receive_msg(received, _size(MSG_USERS_LIST), USERS_LIST_TYPE, &handle_users_list);
        receive_msg(received, _size(MSG_USERS_LIST), ROOMS_LIST_TYPE, &handle_rooms_list);
        usleep(100);
    }
    free(received);
}

void handle_response(void* received, int msg_type){
    MSG_RESPONSE response = *(MSG_RESPONSE*)received;
    char buffer[100];
    switch (response.response_type) {
        case PING:{
            MSG_REQUEST answer;
            answer.type = REQUEST;
            answer.request_type = PONG;
            strcpy(answer.user_name, USERNAME);
            msgsnd(SERVER_CONNECTION, &answer, _size(MSG_REQUEST), 0);
            break;
        }
        case MSG_SEND:{
            //we dont do anything
            break;
        }
        case MSG_NOT_SEND:{
            sprintf(buffer, "\n%s%s", KRED, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case ENTERED_ROOM_SUCCESS:{
            sprintf(buffer, "%s%s", KGRN, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case ENTERED_ROOM_FAILED:{
            sprintf(buffer, "%s%s", KRED, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case LEAVE_ROOM_SUCCESS:{
            sprintf(buffer, "%s%s", KGRN, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case LEAVE_ROOM_FAILED:{
            sprintf(buffer, "%s%s", KRED, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case CHANGE_ROOM_SUCCESS:{
            sprintf(buffer, "%s%s", KGRN, response.content);
            write(1, buffer, strlen(buffer));
            break;
        }
        case CHANGE_ROOM_FAILED:{
            sprintf(buffer, "%s%s", KRED, response.content);
            write(1, buffer, strlen(buffer));
            break;
        } 
    }
}
void handle_message(void* received, int msg_type){
    MSG_CHAT_MESSAGE message = *(MSG_CHAT_MESSAGE*)received;
    if (message.msg_type == PUBLIC) {
        char buffer[MAX_MSG_LENGTH+50];
        sprintf(buffer, "\n%s[CH][%s][%s]: %s",KYEL, message.send_time, message.sender, message.message);
        write(1, buffer, strlen(buffer));
    } else if (message.msg_type == PRIVATE) {
        char buffer[MAX_MSG_LENGTH+50];
        sprintf(buffer, "\n%s[W][%s][%s]: %s",KMAG, message.send_time, message.sender, message.message);
        write(1, buffer, strlen(buffer));
    }
}
void handle_users_list(void* received, int msg_type){
    MSG_USERS_LIST list = *(MSG_USERS_LIST*)received;
    int i;
    char buffer[50];
    sprintf(buffer, "%sUser list: \n", KGRN);
    write(1, buffer, strlen(buffer));
    for (i=0; i<REPO_SIZE; ++i) {
        if (list.users[i][0] != '\0') {
            sprintf(buffer, "<%s>\n", list.users[i]);
            write(1, buffer, strlen(buffer));
        }
    }
}
void handle_rooms_list(void* received, int msg_type){
    MSG_USERS_LIST rooms_list = *(MSG_USERS_LIST*)received;
    int i;
    char buffer[50];
    sprintf(buffer, "%sRooms list: \n", KGRN);
    write(1, buffer, strlen(buffer));
    for (i=0; i<REPO_SIZE; ++i) {
        if (rooms_list.users[i][0] != '\0') {
            sprintf(buffer, "<%s>\n", rooms_list.users[i]);
            write(1, buffer, strlen(buffer));
        }
    }
}

void interface_loop(void* param){
    
    printf("%sWelcome\n", KBLU);
    
    size_t restrict_size = 512;
    char* line = calloc(restrict_size, sizeof(char));

    while (!EXIT_FLAG){
        if(!LOGGED_IN){
            printf("%sConnecting to server\n", KBLU);
            do {
                if(EXIT_FLAG)
                    break;
                printf("%sEnter valid server number\n%s", KYEL, KMAG);
                scanf("%d", &SERVER_CONNECTION);

            } while (perform_login() != SUCCESS);
        } else {
            printf("%s", KWHT);
            getline(&line, &restrict_size, stdin);
            if(!EXIT_FLAG)
                parseline(line);
        }
    }
    free(line);
}

int perform_login(){
    
    printf("%sEnter username\n%s", KYEL, KMAG);
    scanf("%s", USERNAME);
    USERNAME[9] = '\0';

    if(EXIT_FLAG)
        return FAIL;
    
    int return_flag = FAIL;
    
    MSG_LOGIN login;
    login.type = LOGIN;
    login.ipc_num = CLIENT_QUEUE;
    strcpy(login.username, USERNAME);
    
    void* received = malloc(2048);
    
    time_t time_diff = time(0);
    msgsnd(SERVER_CONNECTION, &login, _size(MSG_LOGIN), 0);
    while( 1 ) {
        if ( msgrcv(CLIENT_QUEUE, received, _size(MSG_RESPONSE), RESPONSE, IPC_NOWAIT) != -1 ){
            MSG_RESPONSE temp = *(MSG_RESPONSE*)received;
            if ( temp.response_type == LOGIN_SUCCESS ) {
                return_flag = SUCCESS;
                LOGGED_IN = TRUE;
                printf("\n%s%s\n", KGRN, (*(MSG_RESPONSE*)received).content);
            } else if ( temp.response_type == LOGIN_FAILED ){
                return_flag = FAIL;
                printf("\n%s%s\n", KRED, (*(MSG_RESPONSE*)received).content);
            }
            break;
        } else if ( time(0) - time_diff > TIMEOUT ) {
            printf("%s\nTimeout, server didnt respond\n", KRED);
            return_flag = FAIL;
            break;
        }
    }
    free(received);
    return return_flag;
}



void receive_msg(void* received, int msg_size, int msg_type, fnc_handler handler) {
    if ( msgrcv(CLIENT_QUEUE, received, msg_size, msg_type, IPC_NOWAIT) != -1 ) {
        handler(received, msg_type);
    }
}

void parseline(char* line){
    if ( cmp(line, "/quit", 5) ) {
        perform_logout();
        clean_exit();
        exit(EXIT_SUCCESS);
        
    } else if ( cmp(line, "/join ", 6) ) {
       
        char* roomname = (char*)malloc(ROOM_NAME_MAX_LENGTH*sizeof(char));
        roomname = strndup(line+6, strlen(line)-7);
        perform_room_action(ENTER_ROOM, roomname);
        free(roomname);
       
    } else if ( cmp(line, "/change ", 8) ) {
        
        char* roomname = (char*)malloc(ROOM_NAME_MAX_LENGTH*sizeof(char));
        roomname = strndup(line+8, strlen(line)-9);
        perform_room_action(CHANGE_ROOM, roomname);
        free(roomname);
        
    } else if ( cmp(line, "/leave", 6) ) {
       
        perform_room_action(LEAVE_ROOM, "");
       
    } else if ( cmp(line, "/msg ", 5) ) {
        
        char* name_pos = strchr(line, ' ');
        char* message_pos = strchr(name_pos+1, ' ');
        char* receiver = strndup(name_pos+1, strlen(name_pos)-strlen(message_pos)-1);
        char* message = strndup(message_pos+1, strlen(message_pos)-2);
        
        receiver[USER_NAME_MAX_LENGTH-1] = '\0';
        message[MAX_MSG_LENGTH-1] = '\0';

        send_message(receiver, message, PRIVATE);
        free(receiver);
        free(message);
       
    } else if ( cmp(line, "/help", 5) ) {
        
        display_help();
        
    } else if ( cmp(line, "/rooms", 6) ) {
       
        display_list(ROOMS_LIST);
        
    } else if ( cmp(line, "/users", 6) ) {
       
        display_list(USERS_LIST);
       
    } else if ( cmp(line, "/signout", 8)) {
        
        perform_logout();
    } else if ( cmp(line, "/all ", 4)){
        
        char* msg = strchr(line, ' ');
        char* message = strndup(msg+1, strlen(line)-6);
        send_message(CHANNEL, message, PUBLIC);
        free(message);
    }
}

void perform_logout(){
    MSG_LOGIN login;
    login.type = LOGOUT;
    login.ipc_num = CLIENT_QUEUE;
    strcpy(login.username, USERNAME);
    msgsnd(SERVER_CONNECTION, &login, _size(MSG_LOGIN), 0);
    LOGGED_IN = FALSE;
}

void display_help(){
    printf("%sUse one of the following commands\n", KCYN);
    printf("%s/join <channel_name> \t\tJoins specified channel\n", KCYN);
    printf("%s/leave \t\t\t\tLeaves current channel \n", KCYN);
    printf("%s/msg <username> <message> \tSends private message\n", KCYN);
    printf("%s/rooms \t\t\t\tDisplays list of available rooms\n", KCYN);
    printf("%s/users \t\t\t\tDisplays list of all users\n", KCYN);
    printf("%s/signout \t\t\tSigns out\n", KCYN);
    printf("%s/all <message> \t\t\tSends public message\n", KCYN);

}

void perform_room_action(int action_type, const char* roomname){
    MSG_ROOM room_request;
    
    room_request.type = ROOM;
    room_request.operation_type = action_type;
    strcpy(room_request.room_name, roomname);
    strcpy(room_request.user_name, USERNAME);
    
    strcpy(CHANNEL, roomname);
    
    msgsnd(SERVER_CONNECTION, &room_request, _size(MSG_ROOM), 0);
}

void send_message(const char* receiver, const char* content, int msg_type){
    MSG_CHAT_MESSAGE message;
    message.type = MESSAGE;
    message.msg_type = msg_type;
    strcpy(message.receiver, receiver);
    strcpy(message.sender, USERNAME);
    strcpy(message.message, content);
    
    time_t curtime;
    time(&curtime);
    strncpy(message.send_time, ctime(&curtime)+11, 5);
    message.send_time[5] = '\0';
    
    msgsnd(SERVER_CONNECTION, &message, _size(MSG_CHAT_MESSAGE), 0);
}

void display_list(int request_type){
    MSG_REQUEST rm_ls_req;
    rm_ls_req.type = REQUEST;
    rm_ls_req.request_type = request_type;
    strcpy(rm_ls_req.user_name, USERNAME);
    
    msgsnd(SERVER_CONNECTION, &rm_ls_req, _size(MSG_REQUEST), 0);
}
