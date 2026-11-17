#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>

/*

Flow of a server socket:
1. getaddrinfo
2. socket
3. bind
4. listen
5. accept
6. send & recv

*/

#define BACKLOG 5
#define PORT "3490"

int bind_server() {
  // loading address structs with getaddrinfo
  struct addrinfo hint, *res, *p;
  int yes = 1;
  int rv;

  // socket file descriptor
  int sockfd;

  memset(&hint, 0, sizeof(hint));
  hint.ai_family = AF_UNSPEC;
  hint.ai_socktype = SOCK_STREAM;
  hint.ai_flags = AI_PASSIVE; // fill in my IP for me

  if ((rv = getaddrinfo(NULL, PORT, &hint, &res)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  // looping through all available results and binding to the first address
  for (p = res; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(res);
  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  return sockfd;
}

// function to handle child processes
void sigchild_handler(int s) {
  (void)s;
  int saved_errno = errno;
  while (waitpid(-1, NULL, WNOHANG) > 0);
  errno = saved_errno;
}

// gets address
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

void get_client_ip(struct sockaddr *client_addr, char *s) {
  inet_ntop(client_addr->sa_family, get_in_addr(client_addr), s, sizeof s);
  printf("server: got connection from: %s\n", s);
}

void accept_loop(int sockfd) {
  int new_fd;
  struct sockaddr_storage client_addr;
  socklen_t client_addrsize;
  char s[INET6_ADDRSTRLEN];
  struct sigaction sa;

  sa.sa_handler = sigchild_handler; // kills all dead processes;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("server: sigaction");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (1) {
    client_addrsize = sizeof client_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &client_addrsize);
    if (new_fd == -1) {
      perror("server: accept");
      continue;
    }

    get_client_ip((struct sockaddr *)&client_addr, s);

    if (!fork()) {
      // in child process
      close(sockfd); // child doesn't need this
      char *message = "Hello World!";
      if (send(new_fd, message, strlen(message), 0) == -1) {
        perror("server: send");
      }
      close(new_fd);
      exit(0);
    }

    close(new_fd); // parent doesn't need this
  }
}

int main(int argc, char *argv[]) {

  if (argc != 2) {
    printf("Usage: %s <hostname>\n", argv[0]);
    return -1;
  }

  char *hostname = malloc(256);
  gethostname(hostname, 256);
  printf("server hostname : %s\n", hostname);
  int sockfd = bind_server();

  // listen on the port socket is binded to. queue up n=5 connection requests
  // and block all other
  if (listen(sockfd, BACKLOG) == -1) {
    fprintf(stderr, "listen error: %s\n", strerror(errno));
    return 0;
  }

  printf("waiting for connections...\n");

  // accept loop
  accept_loop(sockfd);

  return 0;
}
