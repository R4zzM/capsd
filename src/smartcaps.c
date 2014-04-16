#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dbg.h"
#include "sys/types.h"
#include <unistd.h>
#include <fcntl.h>

#define MAX_HANDLERS 20
#define FILENAME_LEN 32

/* Forward declarations */ 
int devices_event_handlers(int *handlers, int *nhandlers, int max);

static void await_keypress(int *fd, int nfd)
{
  fd_set readfds; 
  int i;
  int maxfd;
  int ret;
   
  FD_ZERO(&readfds);

  maxfd = 0; 
  for(i = 0; i < nfd; i++) {
    FD_SET(fd[i], &readfds);
    if(fd[i] > maxfd)
      maxfd = fd[i];
  }

  while(1) {
    ret = select(maxfd + 1, &readfds, NULL, NULL, NULL);
    if(ret == -1) {
      log_warn("select() returned with error.");
      continue;
    }
    for(i = 0; i < nfd; i++) {
      if(FD_ISSET(fd[i], &readfds)) {
        debug("Got keypress for fd = %d! Inteagrate with readkbd.c", fd[i]);
        /* Data will need to be read here or select(...) will be spinning */
      }
    }
  }
}

static int open_files(int *fd, int *handlers, int nhandlers)
{
  char filename[FILENAME_LEN];
  int nfiles = 0;
  int i;

  for(i = 0; i < nhandlers; i++) {
    snprintf(filename, FILENAME_LEN, "/dev/input/event%d", handlers[i]);
    fd[i] = open(filename, O_RDONLY);
    if(fd[i] == -1) {
      log_warn("Could not open handler %s", filename);
    } else {
      nfiles++;
      debug("Opened handler: %s", filename);
    }
  }
  
  return nfiles;
}

int main(int argc, char **argv)
{
  int handlers[MAX_HANDLERS];
  int fds[MAX_HANDLERS];
  int nfds;
  int nhandlers;
  int ret;

  ret = devices_event_handlers(handlers, &nhandlers, MAX_HANDLERS);
  if(ret) {
    printf("Error in devices_event_handlers: %s", strerror(errno));
    goto error;
  }
   
  nfds = open_files(fds, handlers, nhandlers);
  check(nfds, "Could not open any file descriptors. Quitting.");

  await_keypress(fds, nfds);

  return 0;

error:
  return errno;
}
