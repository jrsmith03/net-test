#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

// return the open client file descriptor
int setup_connection() {
    struct addrinfo *server_inf;
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.socktype = SOCK_STREAM;
    getaddrinfo()

}

int server_session() {

}