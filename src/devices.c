#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINES 512
#define MAX_LINE_LENGTH 512
#define MAX_HANDLERS 20

static int has_kbd_handler(char *line)
{
  if(line[0] == 'H' && line[1] == ':' && strstr(line, "kbd"))
    return 1;
  else
    return 0;
}

// Returns the X in "eventX" in the line or -1 if the substring doesn't exist
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

int devices_event_handlers(int *handlers, int *n, int nmax)
{
  char *abspath = "/proc/bus/input/devices";
  FILE *devices;
  char line[MAX_LINE_LENGTH];
  int num;
  int i = 0;

  devices = fopen(abspath, "r");
  if(!devices) {
    printf("Error when opening %s: %s\n", abspath, strerror(errno));
    goto err;
  }

  while(fgets(line, MAX_LINE_LENGTH, devices) && i < nmax) {
    if(has_kbd_handler(line)) {
      num = ev_handler_num(line);
      if(num == -1) 
        continue;

      handlers[i] = num;
      i++;
    }
  }
  *n = i;

  return 0;

err:
  return errno;
}

/* int main(int argc, char **argv) */
/* { */
/*   FILE *devices; */
/*   int handlers[MAX_HANDLERS]; */
/*   int nhandlers; */
/*   int i; */


/*   nhandlers = get_ev_handlers(handlers, MAX_HANDLERS, devices); */
  
/*   for(i = 0; i < nhandlers; i++) */
/*     printf("Kbd eventfile: /dev/input/event%d\n", handlers[i]); */
  
/*   return 0; */

/* err: */
/*   return errno; */
/* } */
