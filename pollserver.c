#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT "3490"

int get_listener(void) {

  struct addrinfo hints, *res, *p;
  int rv, numbytes, yes = 1;
  int sockfd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, PORT, &hints, &res)) != 0) {
    fprintf(stderr, "getaddrinfo %s\n", gai_strerror(rv));
    return -1;
  }

  for (p = res; p != NULL; p = p->ai_next) {
    sockfd = socket(p->ai_family, p->ai_socktype, 0);
    if (sockfd == -1) {
      perror("server : socket\n");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
      perror("socket : setsockopt");
      /*
        always remember to close sockfd if this fails
        otherwise it leaks
      */
      close(sockfd);
      continue;
    }

    if ((bind(sockfd, p->ai_addr, p->ai_addrlen)) == -1) {
      perror("server : bind\n");
      close(sockfd);
      continue;
    }

    break;
  }

  freeaddrinfo(res);
  if (p == NULL) {
    return -1;
  }

  if (listen(sockfd, 10) == -1) {
    perror("server : listen\n");
    close(sockfd);
    return -1;
  }

  return sockfd;
}

void add_to_pfds(int sockfd, struct pollfd **pfds, int *count, int *size) {
  if (*count == *size) {
    *size *= 2;
    *pfds = realloc(*pfds, sizeof(**pfds) * (*size));
  }

  (*pfds)[*count].fd = sockfd;
  (*pfds)[*count].events = POLLIN;
  (*pfds)[*count].revents = 0;

  (*count)++;
}

void del_from_pfds(int i, struct pollfd pfds[], int *count) {
  pfds[i] = pfds[*count - 1];
  (*count)--;
}

void print_connection_ip(struct sockaddr *addr) {
  char ipstr[INET6_ADDRSTRLEN];
  if(addr->sa_family == AF_INET6){
    inet_ntop(addr->sa_family, &(((struct sockaddr_in6 *)addr)->sin6_addr), ipstr, sizeof ipstr);
  } else if(addr->sa_family == AF_INET){
    inet_ntop(addr->sa_family, &(((struct sockaddr_in *)addr)->sin_addr), ipstr, sizeof ipstr);
  } else {
    printf("server from unknown family\n");
    return;
  }
  printf("Listening to server : %s\n", ipstr);
  return;
}

void handle_new_connection(int sockfd, struct pollfd **pfds, int *count,
                           int *size) {

  struct sockaddr_storage remoteaddr = {0};
  socklen_t addrlen = sizeof remoteaddr;
  int new_fd = accept(sockfd, (struct sockaddr *)&remoteaddr, &addrlen);
  if (new_fd == -1) {
    perror("server: new connection");
    return;
  } else {
    add_to_pfds(new_fd, pfds, count, size);
    print_connection_ip((struct sockaddr *)&remoteaddr);
  }
}

void handle_client_data(int *pfd_i, int listener, struct pollfd *pfds, int *count){
  
  char buf[256];
  int sender_fd =pfds[*pfd_i].fd;

  int nbytes = recv(sender_fd, buf, sizeof buf - 1, 0);
  if(nbytes <= 0){
    if(nbytes == 0)
      printf("connection lost by client socket %d hung up\n", sender_fd);
    else
      perror("server: client data");
    
    close(sender_fd);
    del_from_pfds(*pfd_i, pfds, count);
    (*pfd_i)--;
  } else {
    buf[nbytes] = '\0';
    printf("received message: %s from client socket: %d\n", buf,sender_fd);

    for(int i = 0; i < *count; i++){
      int dest_fd = pfds[i].fd;
      if(dest_fd != listener && dest_fd != sender_fd){
        if(send(pfds[i].fd, buf, nbytes, 0) == -1)
          perror("server : send");  
      }
    }
  }
}

void process_connections(int sockfd, int *count, int *size,
                        struct pollfd **pfds) {
  for (int i = 0; i < *count; ++i) {
    if ((*pfds)[i].revents & (POLLIN | POLLHUP)) {
      if ((*pfds)[i].fd == sockfd) {  // found a new socket
        handle_new_connection(sockfd, pfds, count, size);
      } else {  // get data from existing socket
        handle_client_data(&i, sockfd, *pfds, count);
      }
    }
  }
}

int main(void) {

  int listener = get_listener();
  if(listener == -1){
    perror("server: get_listener");
    return -1;
  }
  int fd_size = 5;
  int fd_count = 0;
  struct pollfd *pfds = malloc(sizeof *pfds * fd_size);
  if(pfds == NULL){
    perror("malloc");
    exit(1);
  }
  pfds[0].fd = listener;
  pfds[0].events = POLLIN;
  fd_count++;

  while (1) {
    int poll_count = poll(pfds, fd_count, -1);
    if (poll_count == -1) {
      perror("poll");
      exit(1);
    }

    process_connections(listener, &fd_count, &fd_size, &pfds);
  }
}
