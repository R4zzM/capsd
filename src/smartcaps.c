#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dbg.h"
#include "sys/types.h"
#include "unistd.h"
#include <linux/input.h>
#include <sys/poll.h>
#include <fcntl.h>

#define MAX_HANDLERS 20
#define FILENAME_LEN 32

struct kbdstate {
    struct pollfd *pfds;
    int capsdown;
    int lastcode;
};

/* devices.c */ 
int devices_event_handlers(int *handlers, int *nhandlers, int max);

/* inject.c */
void inject_escape_down(int injectfd);
void inject_escape_up(int injectfd);
void inject_lctrl_up(int injectfd);
void inject_lctrl_down(int injectfd);
int inject_init(void);

static int read_event(struct pollfd *pfds, struct input_event *ev) 
{
  int nbytes;

  nbytes = read(pfds->fd, ev, sizeof(*ev));
  if(nbytes == -1) {
    if(errno == EAGAIN || errno == EWOULDBLOCK) {
      debug("No more data to read.");
      goto out;
    } else {
      goto error;
    }
  }
  
  /* Unsure if this can happen */
  check(nbytes == sizeof(*ev), "Full input_event struct could not be read.");

  debug("ev.type = %u, ev.code = %u, ev.value = %u\n", ev->type, ev->code, 
      ev->value);

out:
  return 0;

error:
  return -1;
}

static void handle_keypress(int injectfd, struct kbdstate s, 
    struct input_event ev) {
  if(ev.type == EV_KEY && ev.code == KEY_CAPSLOCK && ev.value == 0) {
    inject_lctrl_up(injectfd);
    if(s.lastcode == KEY_CAPSLOCK) {
      inject_escape_down(injectfd);
      inject_escape_up(injectfd);
    } 
  } else if(ev.type == EV_KEY && ev.code == KEY_CAPSLOCK && ev.value == 1) {
    inject_lctrl_down(injectfd);
  }
}

static void update_kbdstate(struct kbdstate *s, struct input_event ev) {
  debug("Updating kbdstate...");
  if(ev.type == EV_KEY) {
    s->lastcode = ev.code;
    if(ev.code == KEY_CAPSLOCK) {
      s->capsdown = ev.value;  
    }
  }
}

static void await_keypress(int injectfd, struct pollfd *pfds, int npfds)
{
  int i;
  int ret;
  struct input_event ev;
  struct kbdstate s[npfds];

  /* init all kbds */
  for(i = 0; i < npfds; i++) {
    s[i].pfds = &pfds[i];
    s[i].capsdown = 0;
    s[i].lastcode = 0;
  }
              
  while(1) {
    debug("Waiting for input...");
    ret = poll(pfds, npfds, -1);
    if(!ret) {
      log_err("Could not poll file!");
      continue;
    } else if (ret == -1) {
      log_err("Poll returned with an error: %s", strerror(errno));
      continue;
    }
    debug("Reading input from %d files.", ret);
    for(i = 0; i < npfds; i++) {
      if((pfds[i].revents & POLLIN)) {
        debug("Got keypress for fd%d", pfds[i].fd);
        ret = read_event(&pfds[i], &ev);
        if(ret == -1) {
          log_err("Error when reading event: %s", strerror(errno));
          continue;
        }
        handle_keypress(injectfd, s[i], ev);
        update_kbdstate(&s[i], ev);
      }
    }
  }
}

static int open_files(struct pollfd *pfds, int *handlers, int nhandlers)
{
  char filename[FILENAME_LEN];
  int npfds = 0;
  int i;

  for(i = 0; i < nhandlers; i++)
    debug("handlers[%d] = %d", i, handlers[i]);

  for(i = 0; i < nhandlers; i++) {
    snprintf(filename, FILENAME_LEN, "/dev/input/event%d", handlers[i]);
    pfds[i].fd = open(filename, O_RDONLY | O_NONBLOCK);
    if(pfds[i].fd == -1) {
      log_warn("Could not open handler %s", filename);
      i--;
    } else {
      pfds[i].events = POLLIN;
      npfds++;
      debug("Opened handler: %s, fd = %d, i = %d", filename, pfds[i].fd, i);
    }
  }
  
  return npfds;
}

int main(int argc, char **argv)
{
  int handlers[MAX_HANDLERS];
  struct pollfd pfds[MAX_HANDLERS];
  int npfds;
  int nhandlers;
  int ret;
  int injectfd;

  ret = devices_event_handlers(handlers, &nhandlers, MAX_HANDLERS);
  if(ret) {
    printf("Error in devices_event_handlers: %s", strerror(errno));
    goto error;
  }

  npfds = open_files(pfds, handlers, nhandlers);
  check(npfds, "Could not open any file descriptors. Quitting.");

  injectfd = inject_init();
  if(injectfd == -1) {
    log_err("Error when calling init_inject(): %s", strerror(errno));
    goto error;
  }

  await_keypress(injectfd, pfds, npfds);

  return 0;

error:
  return errno;
}
