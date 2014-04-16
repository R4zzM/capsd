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

static int open_files(int *fd, int *handlers, int nhandlers)
{
  char filename[FILENAME_LEN];
  int nfiles = 0;
  int i;

  for(i = 0; i < nhandlers; i++) {
    snprintf(filename, FILENAME_LEN, "/dev/input/event%d", handlers[i]);
    debug("Opening handler: %s", filename);
    fd[i] = open(filename, O_RDONLY);
    if(fd[i] == -1)
      log_warn("Could not open handler %s", filename);
    else
      nfiles++;
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

  return 0;

error:
  return errno;
}
