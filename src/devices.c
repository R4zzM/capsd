#include "dbg.h"
#include "smartcaps.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/ioctl.h>
#include <linux/input.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LINES 512
#define MAX_LINE_LENGTH 512
#define MAX_HANDLERS 20

static int has_kbd_handler(char *line)
{
  if(line[0] == 'H' && line[1] == ':' && strstr(line, "sysrq"))
    return 1;
  else
    return 0;
}

/* Returns the X in the "eventX" or -1 if the substring doesn't exist */
static int ev_handler_num(char *line)
{
  char *name;
  int num = -1;

  name = strstr(line, "event");
  if(name != NULL) {
    num = strtol(name + 5, NULL, 10);
  }

  return num;
}

static int ev_handler_nums(int *handlers, int nmax)
{
  char *abspath = "/proc/bus/input/devices";
  FILE *devices;
  char line[MAX_LINE_LENGTH];
  int num;
  int i = 0;

  devices = fopen(abspath, "r");
  check(devices, "Error when opening %s", abspath);

  while(fgets(line, MAX_LINE_LENGTH, devices) && i < nmax) {
    if(has_kbd_handler(line)) {
      num = ev_handler_num(line);
      if(num == -1) 
        continue;

      handlers[i] = num;
      i++;
    }
  }

  fclose(devices);
  return i;

error:
  fclose(devices);
  return 0;
}

int devices_init(struct kbdstate *s, int maxhandlers)
{
  char filename[32];
  int npfds;
  int fd;
  int i;
  int handlers[maxhandlers];
  int nhandlers;
  
  memset(handlers, 0, maxhandlers); 
  nhandlers = ev_handler_nums(handlers, maxhandlers);

  npfds = 0;
  for(i = 0; i < nhandlers; i++) {
    snprintf(filename, 32, "/dev/input/event%d", handlers[i]);
    fd = open(filename, O_RDONLY | O_NONBLOCK);
    if(fd == -1) {
      log_warn("Could not open %s", filename);
    } else {
      // Grab the device for exclusive use
      if(handlers[i] == 2){
        if(ioctl(fd, EVIOCGRAB, 1) == -1) {
          log_warn("Could not grab device %s. %s", filename, strerror(errno));
          close(fd);
          continue;
        }
      }
      s->pfds[npfds].fd = fd;
      s->pfds[npfds].events = POLLIN;
      debug("Opened: %s, fd = %d", filename, s->pfds[npfds].fd);
      npfds++;
    }
  }

  s->npfds = npfds;

  return npfds;
}
