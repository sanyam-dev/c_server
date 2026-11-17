#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "3490"
#define MAXBUFFERSIZE 1024

void *get_ip_addr(struct sockaddr *s) {
  if (s->sa_family == AF_INET6)
    return &(((struct sockaddr_in6 *)s)->sin6_addr);

  return &(((struct sockaddr_in *)s)->sin_addr);
}

int main() {
  struct addrinfo hints, *res, *p;
  int yes = 1, rv, sockfd, clientfd;
  struct sockaddr_storage client_addr;
  char s[INET6_ADDRSTRLEN];
  char buf[MAXBUFFERSIZE];
  socklen_t client_addr_size;
  int recvbytes = 0;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags = AI_PASSIVE;

  rv = getaddrinfo(NULL, PORT, &hints, &res);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo error : %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = res; p != NULL; p = p->ai_next) {

    sockfd = socket(p->ai_family, p->ai_socktype, 0);
    if (sockfd == -1) {
      perror("error getting socket");
      continue;
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("listener: bind error\n");
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "listener: failed to bind socket\n");
    return 2;
  }

  freeaddrinfo(res);

  // no accept loop, we wait for some client to send packet
  printf("listening for connection...\n");

  client_addr_size = sizeof client_addr;
  if ((recvbytes = recvfrom(sockfd, buf, MAXBUFFERSIZE - 1, 0,
                            (struct sockaddr *)&client_addr,
                            &client_addr_size)) == -1) {
    perror("listener: recvfrom");
    exit(1);
  }
  printf("listener: got packet from %s\n",
         inet_ntop(client_addr.ss_family,
                   get_ip_addr((struct sockaddr *)&client_addr), s, sizeof s));
  printf("listener: packets receieved: %d bytes long\n", recvbytes);
  buf[recvbytes] = '\0';
  printf("Listener: got packet: %s\n", buf);

  close(sockfd);

  return 0;
}
