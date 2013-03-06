//
//  main.c
//  unix-communicator-client
//
//  Created by Maciej Żurad on 1/26/13.
//  Copyright (c) 2013 Maciej Żurad. All rights reserved.
//

#include "protocol.h"
#include "client.h"

#define cmp(line, text, nr, letter) !(strncmp(line, text, nr) && strncmp(line, letter, 2)) 

void init_client_resources(){
    SHARED_MEM = malloc(2048);
}

void init_client(){

    EXIT_FLAG = FALSE;
    
    init_client_resources();
    CLIENT_QUEUE = msgget(IPC_PRIVATE, 0666);
    
    signal(SIGINT, clean_resources);
}

void clean_resources(){
    printf("\nFreeing up resources\n");
    printf("Quitting\n");
    msgctl(CLIENT_QUEUE, IPC_RMID, 0);
    free(SHARED_MEM);
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
//        receive_msg(received, _size(MSG_RESPONSE), RESPONSE, &handle_response);
//        receive_msg(received, _size(MSG_CHAT_MESSAGE), MESSAGE, &handle_message);
//        receive_msg(received, _size(MSG_USERS_LIST), USERS_LIST_TYPE, &handle_users_list);
//        receive_msg(received, _size(MSG_USERS_LIST), ROOMS_LIST_TYPE, &handle_rooms_list);
        usleep(100);
    }
    free(received);
}

void handle_response(void* received, int msg_type){

}
void handle_message(void* received, int msg_type){

}
void handle_users_list(void* received, int msg_type){

}
void handle_rooms_list(void* received, int msg_type){

}

void interface_loop(void* param){
    printf("Connecting to server\n");
    do {
        printf("Enter valid server number\n");
        scanf("%d", &SERVER_CONNECTION);
    } while (perform_login() != SUCCESS );
    
    size_t restrict_size = 512;
    char* line = calloc(restrict_size, sizeof(char));
    while(1){
            getline(&line, &restrict_size, stdin);
            if(EXIT_FLAG)
                break;
            else
                parseline(line);
    }
    free(line);
}

int perform_login(){
    printf("Enter username\n");
    scanf("%s", USERNAME);
    
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
            if ( temp.response_type == LOGIN_SUCCESS )
                return_flag = SUCCESS;
            else if ( temp.response_type == LOGIN_FAILED )
                return_flag = FAIL;
            printf("%s\n", (*(MSG_RESPONSE*)received).content);
            break;
        } else if ( time(0) - time_diff > TIMEOUT ) {
            printf("Timeout, server didnt respond\n");
            return_flag = FAIL;
            break;
        }
    }
    free(received);
    return return_flag;
}

void receive_msg(void* received, size_t msg_size, int  msg_type, fnc_handler handler) {
    if ( msgrcv(CLIENT_QUEUE, received, msg_size, msg_type, IPC_NOWAIT) != -1 ) {
        handler(received, msg_type);
    }
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