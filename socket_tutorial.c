#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <stdlib.h>

void get_ip_addr(const char *PORT);

int main(int argc, char *argv[]){
    
    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    const uint16_t PORT = (uint16_t)atoi(argv[1]);  // gets port from argument

    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0) )<= 0){
        printf("Socket creation failed\n");
        return -1;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    // To accept any IP address in server
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    const char *pt = argv[1];
    get_ip_addr(pt);

    return 0;
}



void get_ip_addr(const char *PORT){
    
    // love this thing about c, manual cleanups!

    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    
    // specifying structure for addrinfo results from getaddrinfo(...)
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP Stream Socket
    hints.ai_flags = AI_PASSIVE;    // fill in my IP for me
    
    int status;
    struct addrinfo *servnow;   // points to results
    if((status = getaddrinfo(NULL, PORT, &hints, &servnow)) != 0){
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return;
    }
    
    char ipres[INET6_ADDRSTRLEN];   // string to hold ip result

    // addrinfo: prepare socket address structure for subsequent use. 
    //           also used in host name lookups and service name lookups.
    struct addrinfo *itr = servnow; 

    while(itr != NULL){
        void *addr; // void * holds any type of address, used as generic pointer. 
        char *ipver; 
    
        if(itr->ai_family==AF_INET){
            ipver = "IPv4";
            addr = &((struct sockaddr_in *) itr->ai_addr)->sin_addr;    // sockaddr_in : parallel sockaddr structure used to store socket address information. 
                                                                        //               only used with IPv4. IPv6 uses sockaddr_in6.
                                                                        // can be cast to sockaddr, thus can be used in place of sockaddr. ex : connect()                                                                        
        } else {
            ipver = "IPv6";
            addr = &((struct sockaddr_in6 *) itr->ai_addr)->sin6_addr;  // sockaddr_in6 : used to store IPv6 socket address information. 
                                                                        // sockaddr_storage: another parallel structure to be used when 
                                                                        // you want to be address agnostic.
        }
        inet_ntop(itr->ai_family, addr, ipres, sizeof(ipres));
        printf("Version: %s | Address %s\n", ipver, *ipres);

        itr = itr->ai_next;
    }

    freeaddrinfo(servnow);
    return;
}