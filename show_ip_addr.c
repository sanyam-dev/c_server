#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[]){
    if (argc != 2) {
        printf("Usage: %s <hostname>\n", argv[0]);
        return 1;
    }

    // show ip address for given hostname

    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    hint.ai_family=AF_UNSPEC;
    hint.ai_socktype=SOCK_STREAM;

    struct addrinfo *res; 
    int status;

    if((status=getaddrinfo(argv[1], NULL, &hint, &res)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return -1;
    }


    struct addrinfo *itr = res;

    char ipres[INET6_ADDRSTRLEN];
    while(itr != NULL){
        
        void *addr;
        char *ipver; 

        if(itr->ai_family == AF_INET){
            addr = &((struct sockaddr_in *)itr->ai_addr)->sin_addr;
            ipver = "ipv4";
        } else{
            addr = &((struct sockaddr_in6 *)itr->ai_addr)->sin6_addr;
            ipver = "ipv6";
        }
        inet_ntop(itr->ai_family, addr, ipres, sizeof(ipres));
        printf("Version : %s | Address : %s\n", ipver, ipres);
        itr = itr->ai_next;
    }

    freeaddrinfo(itr);
    freeaddrinfo(res);

    return 0;
}
