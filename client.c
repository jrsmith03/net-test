// Client.c

#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <unistd.h>


int main(int argc, char** argv) {
    // Establish an IPV4 socket file descriptor using TCP
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("Given descriptor: %d\n", sockfd);
    // First argument is the address for which to bind the socket to.
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = atoi(argv[2]);
    struct in_addr sin_addr;
    sin_addr.s_addr = inet_addr(argv[1]); 
    addr.sin_addr = sin_addr; 

    if (connect(sockfd, &addr, sizeof(addr)) == -1) {
        printf("Connect failed!\n");
    } else {
        switch(atoi(argv[3])) {
            case 1:
                while(1) {
                    char in[1];
                    read(fileno(stdin), in, 1);
                    printf("input: %s\n", in);
                    write(sockfd, in, 1);
                }
                break;
            case 2:
                char file_name[32]; 
                printf("Enter name of file to copy...\n");
                scanf("%s", file_name);
                int copyfd = open(file_name, 0);
                if (copyfd != -1) {
                    // Give the server a file name
                    write(sockfd, file_name, 32);
                    
                    char file_byte[1]; 
                    int fb = read(copyfd, file_byte, 1);
                    while(fb != 0 && fb != -1) {
                        write(sockfd, file_byte, 1);
                        fb = read(copyfd, file_byte, 1);
                    }
                    printf("Transfer complete.\n");
                } else {
                    printf("The file %s does not exist.\n");
                }
                close(sockfd);
                break;
        }
    }

    return 0;
}