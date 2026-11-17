#include<stdio.h>
#include<sys/types.h>
#include<netdb.h>
#include<errno.h>
#include<sys/socket.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <hostname>\n", argv[0]);
        return -1;
    }

    int status; 
    struct addrinfo hints;
    struct addrinfo *servinfo, *p;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    status = getaddrinfo(argv[1], argv[2], &hints, &servinfo);

    if(status != 0){
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return 1;
    } 

    int sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol);
    if(sockfd == -1){
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 2; 
    }
    // bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen); doesn't needs to be called when we are using connect()

    int connect_status = connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
    if(connect_status == -1){
        fprintf(stderr, "connect error: %\n", strerror(errno));
        return 3;
    }

    



}