//
//  main.c
//  webserver
//
//  Copyright © 2021 Nadia Candra. All rights reserved.
//
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <unistd.h>  
#include <arpa/inet.h> 
#include <fcntl.h> 
#include <netdb.h> 
#include <sys/stat.h> 
#include <pthread.h>
#include <semaphore.h>

#define SERVER_PORT 18000

#define MAXLINE 2048
#define SA struct sockaddr
#define BUFFER_SIZE 1024
#define CONNECTION_NUMBER 10


char* in (char* buf, int fd_client);
void* conn (int fd_server, int fd_client, char* buf);
int threadcount = 0;
sem_t mutex; //to control threadcount

char webpage [] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Hello</title>\r\n"
"<style>body {background-color: #FFB6C1 }</style></head>\r\n"
"<body><center><h1>Hello everyone</h1><br>\r\n"
"</center></body></html>\r\n";

char webpage1 [] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Morning</title>\r\n"
"<style>body {background-color: #FFFF00 }</style></head>\r\n"
"<body><center><h1>Good Morning</h1><br>\r\n"
"</center></body></html>\r\n";

char webpage2 [] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Night</title>\r\n"
"<style>body {background-color: #778899 }</style></head>\r\n"
"<body><center><h1>Good Night</h1><br>\r\n"
//"<p style="color:white;">Good Night</p>\r\n"
"</center></body></html>\r\n";

char webpage3 [] =
"HTTP/1.1 200 OK\r\n"
"Content-Type: text/html; charset=UTF-8\r\n\r\n"
"<!DOCTYPE html>\r\n"
"<html><head><title>Error</title>\r\n"
"<body><center><h1>Error</h1><br>\r\n"
"</center></body></html>\r\n";



int main(int argc, const char * argv[]) {
    struct sockaddr_in server_addr, client_addr;
    socklen_t sin_len = sizeof(client_addr);
    int fd_server, fd_client;
    char buf[2048];
    int on = 1;
    int new_socket, c, *new_sock;
    struct sockaddr_in server, client;
    int port;
    //port = *argv[i]
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);
    fd_server = socket(AF_INET, SOCK_STREAM, 0);
    pthread_t sniffer_thread;
    setsockopt(fd_server, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int));

    
    if (fd_server < 0) {
        perror("socket");
        exit(1);   
    }

    if(bind(fd_server, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        close(fd_server);
        exit(1);
    }
    
    if(listen(fd_server, 10) == -1){
        perror("listen");
        close(fd_server);
        exit(1);
    }
    
    while(1) {
        fd_client = accept(fd_server, (struct sockaddr *) &client_addr, &sin_len);
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = new_socket;
        
        /*if (pthread_create (&sniffer_thread, NULL, conn, (void*)new_sock) < 0) {
            puts("Could not create thread");
            return 1;
        } */
        if(fd_client == -1) {
        
            perror ("Connection failed... \n");
            continue;
        } 
         printf("Got client connection..\n");
    
            memset(buf, 0, 2048);
            read(fd_client, buf, 2047);
            in(buf, fd_client);
            
        close(fd_client);
        exit(0);
    }   
}

void* conn(int fd_server, int fd_client, char* buf) {
    int request;
    char client_reply[BUFFER_SIZE], *request_lines[3];
    char *file_name;
    char *extension;
    
    int sock = fd_server;
    request = recv(sock, fd_client, BUFFER_SIZE, 0);

    sem_wait(&mutex);
    threadcount++;

    if(threadcount > 10) {
        char *message = "HTTP/1.0 400 Bad Request\r\nContent-Type: text/html\r\n\r\n"
        "<!doctype html><html><body>System is busy right now.</body></html>\r\n";
        write(sock, message, strlen(message));
        threadcount--;
        sem_post(&mutex);
        free(fd_server);
        shutdown(sock, SHUT_RDWR);
        close(sock);
        sock=-1;
        pthread_exit(NULL);
    } 

    sem_post(&mutex);

    if(request < 0) {
        puts("Fail");
    }
    else if (request == 0) {
        puts("Disconnected unexpectedly.");
    }
    else {
        printf("%s\n", buf);

            write(fd_client, webpage, sizeof(webpage)-1);
            close(fd_client);
            printf("closing....\n");
            exit(0);        
    }
    free(fd_server);
    shutdown(sock, SHUT_RDWR);
    close(sock);

    sock = -1;
    sem_wait(&mutex);
    threadcount--;

    sem_post(&mutex);
    pthread_exit(NULL);
} 

char* in (char* buf, int fd_client) {
        char *str;
        char *str1;
        char *str2;
        char* line;
   
        char* token; 
        char* token1; 
        char* token2;
      
        str = (char *) malloc(sizeof (char));
        str1 = (char *) malloc(sizeof (char));
        str2 = (char *) malloc(sizeof (char));
        token = (char *) malloc(sizeof (char));
        token1 = (char *) malloc(sizeof (char));
        token2 =(char *) malloc(sizeof (char));
        line = (char*) malloc(sizeof(char));

        line = buf;
        printf("buffer %s", buf);
    
        token =  strtok(buf, " ");
        token1 = strtok(NULL, " "); 
        token2 = strtok(NULL, " "); 
        
        strcpy(str, "day");
        strcpy(str1, "night");
        strcpy (str2, "\0");

        if (strcmp(token1+1, str)== 0){
    
            printf("%s\n", buf);

            write(fd_client, webpage1, sizeof(webpage1)-1);
            close(fd_client);
            printf("closing....\n");
            exit(0);        
        } else if(strcmp(token1+1, str1)== 0) {
            printf("%s\n", buf);

            write(fd_client, webpage2, sizeof(webpage2)-1);
            close(fd_client);
            printf("closing....\n");
            exit(0);  
        } else if (strcmp(token1+1, str2)== 0){
            printf("%s\n", buf);
               
            write(fd_client, webpage, sizeof(webpage)-1);
            close(fd_client);
            printf("closing....\n");
            exit(0);  
        } else {
            printf("%s\n", buf);
               
            write(fd_client, webpage3, sizeof(webpage3)-1);
            close(fd_client);
            printf("closing....\n");
            exit(0);
        }

        return 0;

    }