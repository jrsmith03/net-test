// Server.c
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

void* handle_con (void*);

int main(int argc, char** argv) {
    
    // Establish an IPV4 socket file descriptor using TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Given descriptor: %d\n", sockfd);
    // First argument is the address for which to bind the socket to.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = atoi(argv[1]);
    struct in_addr sin_addr;
    sin_addr.s_addr = INADDR_ANY; 
    addr.sin_addr = sin_addr; 

    if (bind(sockfd, &addr, sizeof(addr)) == -1) {
        printf("Binding failed!\n");
    }
    
    // Now we want to make the sockfd a passive socket
    if (listen(sockfd, 255) == -1) {
        printf("Listen failed!\n");
    }

    // I'm not sure about this, but we might want to create a directory that is locked per thread so that there aren't races between client data.
    // But that might defeat the purpose of multithreading.

    // Loop indefinitely until the server recieves SIGINT
    printf("opt %d",atoi(argv[2]));
    switch (atoi(argv[2])) {
        // Support for the client writing directly to the system's stdout
        case 1:
            socklen_t addr_len = sizeof(addr);
            int client_fd = accept(sockfd, &addr, &addr_len);
            printf("new client: %d\n", client_fd);
            while(1) {
                fork();
                execl('/bin/bash', NULL);
                char in[1];
                read(client_fd, in, 1);
                printf ("%s",in);
                write(fileno(stdin), in, 1);
            }
            break;
        // Support for multithreaded transfer of files
        case 2:
            while (1) {
                socklen_t addr_len = sizeof(addr);
                int client_fd = accept(sockfd, &addr, &addr_len);
                if (client_fd != -1) {
                    pthread_t worker;
                    pthread_create(&worker, NULL, handle_con, &client_fd);
                }
            }
            break;
    }
    

    return 0;
}


void* handle_con (void* v_client_id) {
    int client_fd = *(int*)(v_client_id);
    if (client_fd == -1) {
        printf("Unable to accept connection \n");
    } else {
        printf("Worker ID %d; New client connected at %d\n", client_fd);
        
        // Create the file that the client is transferring.
        char file_name[32];
        read(client_fd, file_name, 32);
        printf("Creating new file: %s\n",file_name);
        int transfer_fd = open(file_name, O_RDWR | O_CREAT);
        printf("%d\n", transfer_fd);
        // Transfer the contents from the client. 
        char buf[1];
        ssize_t numread = read(client_fd, buf, 1);
        while(numread == 1) {
            size_t written = write(transfer_fd, buf, 1);
            if (written != 1) {
                printf("Unable to write byte error %d\n", errno);
            }
            numread = read(client_fd, buf, 1);
        }
        printf("Client %d done transferring.\n", client_fd);
        close(client_fd);
    }
}