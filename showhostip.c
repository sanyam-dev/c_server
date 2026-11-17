#include<sys/socket.h>
#include<memory.h>
#include<netdb.h>
#include<stdio.h>
#include<arpa/inet.h>


int get_ip_addr(void *site){
    struct addrinfo hints; 
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo *res, *p; 
    int status;

    if((status=getaddrinfo(site, NULL, &hints, &res)) != 0){
        fprintf(stderr, "getaddrinfo %s\n", gai_strerror(status));
        return 1;
    }

    char ipres[INET6_ADDRSTRLEN];

    for(p = res; p != NULL; p = p->ai_next){
        void *addr; 
        char *ipver; 

        if(p->ai_family == AF_INET){
            addr = &(((struct sockaddr_in *)p->ai_addr)->sin_addr);
            ipver = "IPv4";
        } else{
            addr = &(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr);
            ipver = "IPv6";
        }
        inet_ntop(p->ai_family, addr, ipres, INET6_ADDRSTRLEN);
        printf("Version : %s | IP Address : %s\n", ipver, ipres); 
    }
    freeaddrinfo(res);
    return 0; 
}

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    get_ip_addr(argv[1]);
    return 0; 
}