#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dbg.h"

#define MAX_HANDLERS 20

/* Forward declarations */ 
int devices_event_handlers(int *handlers, int *nhandlers, int max);

int main(int argc, char **argv)
{
  int handlers[MAX_HANDLERS];
  int i;
  int nhandlers;
  int ret;

  ret = devices_event_handlers(handlers, &nhandlers, MAX_HANDLERS);
  if(ret) {
    printf("Error in devices_event_handlers: %s", strerror(errno));
    goto err;
  }
  
  for(i = 0; i < nhandlers; i++)
    debug("Using found kbd handler: /dev/input/event%d", handlers[i]);

  return 0;

err:
  return errno;
}
