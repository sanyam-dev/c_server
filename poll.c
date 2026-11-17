#include <poll.h>
#include <stdio.h>
#include <sys/poll.h>

int main() {
  struct pollfd pfds[1];
  pfds[0].fd = 0;
  pfds[0].revents = POLLIN; // Tell me when data is ready to read at recv

  printf("Press RETURN key in 2.5 seconds\n");
  int num_events = poll(pfds, 1, 2500);

  if (num_events == 0) {
    printf("Poll timed out!");
  } else {
    int pollin_happened = pfds[0].revents & POLLIN;
    if (pollin_happened) {
      printf("File descriptor %d is ready to read\n", pfds[0].fd);
    } else {
      printf("Unexpected event: %d\n", pfds[0].revents);
    }
  }

  return 0;
}
