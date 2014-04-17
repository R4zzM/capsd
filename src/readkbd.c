#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char** argv)
{
  struct input_event ev;  
  int fd[argc + 1];
  int nbytes;
  fd_set readfds;
  int ret;
  int i;
  int evnums[argc + 1];
  char file[32];
  int fdmax = 0;

  if(argc == 1) {
    printf("Need arguments!\n");
    return -1;
  }

  for(i = 0; i < argc; i++) {
    /* No error checking... */
    evnums[i] = strtol(argv[i], NULL, 10);
  }

  FD_ZERO(&readfds);
  /* const char *file = "/dev/input/event5"; */
  for(i = 0; i < argc; i++) {
    snprintf(file, 32, "/dev/input/event%d", evnums[i]);
    fd[i] = open(file, O_RDONLY);
    if(fd[i] == -1) {
      printf("Could not open %s\n\n", file);
      continue;
    }
    FD_SET(fd[i], &readfds);
    printf("Opened file %s. fd = %d\n", file, fd[i]);
    if(fdmax < fd[i]) {
      fdmax = fd[i];
    }
  }
  
  while(1) {
    ret = select(fdmax + 1, &readfds, NULL, NULL, NULL);
    if(ret == -1) {
      printf("select(...) returned with error.");
      continue;
    } else {
      printf("select returned...");
    }
    for(i = 0; i < argc; i++) {
      if(FD_ISSET(fd[i], &readfds)) {
        nbytes = read(fd[i], &ev, sizeof(ev));
        if(nbytes != sizeof(ev)) {
          // Not sure if this is possible.
          printf("Crap. Got %d bytes but expected %lu.\n", nbytes, sizeof(ev));
          goto err_close_file;
        } else {
          printf("ev.type = %u, ev.code = %u, ev.value = %u\n", ev.type, ev.code, 
              ev.value);
        }
      }
    }
  }

  for(i = 0; i < argc; i++) {
    close(fd[i]);
  }
  
  return 0;

err_close_file:
  for(i = 0; i < argc; i++) {
    close(fd[i]);
  }
err:
  return -1;
}
