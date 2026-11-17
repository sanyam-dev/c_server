#include<sys/socket.h>
#include<netdb.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define PORT "3490"
#define MAXBUFSIZE 1024

int main(int argc, char *argv[]){
    int sockfd;
    struct addrinfo hints, *res, *p;
    int rv, numbytes;

    if (argc != 3) {
        fprintf(stderr,"usage: talker hostname message\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;

    rv = getaddrinfo(argv[1], PORT, &hints, &res); 
    if(rv != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }

    for(p = res; p != NULL; p = p->ai_next){
        sockfd = socket(p->ai_family, p->ai_socktype, 0);
        if(sockfd == -1){
            perror("talker: socket");
            continue;
        }
        break;
    }
    if(p == NULL){
        fprintf(stderr, "talker: failed to create socket\n");
        return 2;
    }
    
    if((numbytes = sendto(sockfd, argv[2], strlen(argv[2]), 0, p->ai_addr, p->ai_addrlen)) == -1){
        perror("talker: sendto");
        exit(1);
    }

    freeaddrinfo(res);
    printf("talker: sent %d bytes to %s\n", numbytes, argv[1]);
    close(sockfd);

    return 0;
}