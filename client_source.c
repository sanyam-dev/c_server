#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT "3490"
#define MAXDATASIZE 2048

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

int main(int argc, char *argv[]) {
  struct addrinfo hints, *res, *p;
  int rv, yes = 1;
  int sockfd;
  char s[INET6_ADDRSTRLEN];
  char buf[MAXDATASIZE];

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  rv = getaddrinfo(argv[1], PORT, &hints, &res);
  if (rv != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    exit(1);
  }

  for (p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("client: socket");
      continue;
    }
    /*

        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) ==
       -1){ perror("client: setsockopt"); exit(1);
        }

        we don't need to set socket to be reusable
    */

    inet_ntop(p->ai_family, get_in_addr(p->ai_addr), s, sizeof s);
    printf("client: attempting to connect to %s\n", s);
    //  we don't have to bind either

    if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      perror("client: connect");
      close(sockfd);
      continue;
    }

    break;
  }

  if (p == NULL) {
    fprintf(stderr, "client: failed to connect\n");
    return 2;
  }

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr), s,
            sizeof s);
  printf("client: connected to %s\n", s);
  freeaddrinfo(res);

  rv = recv(sockfd, buf, sizeof buf - 1, 0);
  if (rv == -1) {
    perror("client: recv");
    exit(1);
  }

  buf[rv] = '\0';
  close(sockfd);
  return 0;
}
