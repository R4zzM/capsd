#include <stdio.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  struct input_event ev;  
  int fd;
  int nbytes;
  fd_set readfds;
  int ret;
  
  const char *file = "/dev/input/event18";
  fd = open(file, O_RDONLY);
  if(fd == -1) {
    printf("Could not open %s\n", file);
    goto err;
  }

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  
  while(1) {
    ret = select(fd + 1, &readfds, NULL, NULL, NULL);
    if(ret == -1) {
      printf("select(...) returned with error.");
      continue;
    } else {
      printf("select returned...");
    }
    if(FD_ISSET(fd, &readfds)) {
      nbytes = read(fd, &ev, sizeof(ev));
      if(nbytes != sizeof(ev)) {
        // Not sure if this is possible.
        printf("Crap. Got %d bytes but expected %u.\n", nbytes, sizeof(ev));
        goto err_close_file;
      } else {
        printf("ev.type = %u, ev.code = %u, ev.value = %u\n", ev.type, ev.code, 
            ev.value);
      }
    }
  }
  
  close(fd);

  return 0;

err_close_file:
  close(fd);
err:
  return -1;
}
