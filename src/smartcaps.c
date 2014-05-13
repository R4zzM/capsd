#include "dbg.h"
#include "smartcaps.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <stdio.h>
#include <string.h>
#include <sys/poll.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_HANDLERS 20
#define FILENAME_LEN 32

/* devices.c */ 
int devices_init(struct kbdstate *s, int maxhandlers);

/* inject.c */
void inject_forward_keypress(int fd, int code, int value);
void inject_escape_down(int injectfd);
void inject_escape_up(int injectfd);
void inject_lctrl_up(int injectfd);
void inject_lctrl_down(int injectfd);
int inject_init(void);

static int read_event(struct pollfd pfds, struct input_event *ev) 
{
  int nbytes;

  nbytes = read(pfds.fd, ev, sizeof(*ev));
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
  debug("fd = %d, type = %u, code = %u, value = %u", pfds.fd, ev->type, ev->code, ev->value);

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
  } else if(ev.type == EV_KEY) {
    inject_forward_keypress(injectfd, ev.code, ev.value);
  }
}

static void update_kbdstate(struct kbdstate *s, struct input_event ev) {
  if(ev.type == EV_KEY) {
    s->lastcode = ev.code;
    if(ev.code == KEY_CAPSLOCK) {
      s->capsdown = ev.value;  
    }
  }
}

static void await_keypress(int injectfd, struct kbdstate *s)
{
  int i;
  int ret;
  struct input_event ev;
              
  while(1) {
    debug("Waiting for input...");
    ret = poll(s->pfds, s->npfds, -1);
    if(!ret) {
      log_err("Could not poll file!");
      continue;
    } else if (ret == -1) {
      log_err("Poll returned with an error: %s", strerror(errno));
      continue;
    }
    for(i = 0; i < s->npfds; i++) {
      if((s->pfds[i].revents & POLLIN)) {
        ret = read_event(s->pfds[i], &ev);
        if(ret == -1) {
          log_err("Error when reading event: %s", strerror(errno));
          continue;
        }
        handle_keypress(injectfd, *s, ev);
        update_kbdstate(s, ev);
      }
    }
  }
}

int main(int argc, char **argv)
{
  int injectfd;
  struct kbdstate s;
  struct pollfd pfds[MAX_HANDLERS];
   
  s.pfds = pfds;
  s.npfds = 0;
  s.capsdown = 0;
  s.lastcode = 0;

  s.npfds = devices_init(&s, MAX_HANDLERS);
  check(s.npfds, "No handlers were opened. Exiting.");

  injectfd = inject_init();
  check(injectfd != -1, "Error when initializing injector: %s", 
      strerror(errno));

  /* Enter mainloop */
  log_info("Init successful. Waiting for keyboard input.");
  await_keypress(injectfd, &s);

  return 0;

error:
  return -1;
}
