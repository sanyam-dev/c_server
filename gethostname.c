#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>

void get_hostname(){
    char *hostname = malloc(256);
    gethostname(hostname, 256);
    printf("hostname: %s\n", hostname);
    return;
}

void get_peername(int sockfd, struct sockaddr *addr, size_t addr_len){
    getpeername(sockfd, addr, addr_len);
    printf("peername: %s\n", addr);
    
}

int main(){
    get_hostname();
    return 0;

}