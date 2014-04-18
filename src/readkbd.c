#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
/* #include <unistd.h> */
#include <fcntl.h>
#include <errno.h>
#include <sys/poll.h>

int main(int argc, char** argv)
{
  int nfiles = argc - 1; 
  struct input_event ev;
  int fd[nfiles];
  int nbytes;
  struct pollfd pfds[nfiles];
  int ret;
  int i;
  int j;
  int evnums[nfiles];
  char file[32];
  int fdmax = 0;

  if(nfiles == 0) {
    printf("Need arguments!\n");
    return -1;
  }

  for(i = 0; i < nfiles; i++) {
    /* No error checking... */
    evnums[i] = strtol(argv[i + 1], NULL, 10);
  }

  /* const char *file = "/dev/input/event5"; */
  for(i = 0; i < nfiles; i++) {
    snprintf(file, 32, "/dev/input/event%d", evnums[i]);
    pfds[i].fd = open(file, O_RDONLY | O_NONBLOCK);
    if(pfds[i].fd == -1) {
      printf("Could not open %s\n\n", file);
      goto error;
    }
    pfds[i].events = POLLIN;
  }

  while(1) {
    printf("waiting for input...\n");
    ret = poll(pfds, nfiles, -1);
    if(ret == -1) {
      printf("Poll returned with an error: %s\n", strerror(errno));
      continue;
    } else if (!ret) {
      printf("Could not poll file!\n");
      continue;
    } else {
      printf("poll returned. ret = %d\n", ret);
      for(i = 0; i < nfiles; i++) {
        if(pfds[i].revents & POLLIN) {
          printf("Reading fd%d: ", pfds[i].fd);
          nbytes = read(pfds[i].fd, &ev, sizeof(ev));
          if(nbytes == -1) {
            if(errno == EAGAIN || errno == EWOULDBLOCK) {
              printf("No more data to read.\n");
              continue;
            }
          } else if(nbytes != sizeof(ev)) {
            /* Probably not possible */
            printf("Invalid event structure!\n");
            continue;
          } else {
            printf("ev.type = %d, ev.code = %d, ev.value = %d\n", ev.type,
                ev.code, ev.value);
          }
        }
      }
    }
  }

  for(i = 0; i < nfiles; i++) {
    close(fd[i]);
  }

  return 0;

error:
  return -1;
}
