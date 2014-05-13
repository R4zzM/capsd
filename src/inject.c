#include "dbg.h"
#include "smartcaps.h"
#include <fcntl.h>
#include <linux/input.h>
#include <linux/ioctl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

/* Key event values are not defined in linux/input.h */
#define KEY_VALUE_RELEASE 0
#define KEY_VALUE_PRESS 1
#define KEY_VALUE_REPEAT 2

int inject_event(int fd, int type, int code, int value) 
{
  int ret;
  struct input_event keyevent;
  struct input_event synevent;

  memset(&keyevent, 0, sizeof(keyevent));
  memset(&synevent, 0, sizeof(synevent));

  keyevent.type = type;
  keyevent.code = code;
  keyevent.value = value;

  synevent.type = EV_SYN;
  synevent.code = 0;
  synevent.value = 0;
  
  debug("fd = %d", fd);
  ret = write(fd, &keyevent, sizeof(keyevent));
  check(ret != -1, "Writing event failed: %s", strerror(errno)); 
  ret = write(fd, &synevent, sizeof(synevent));
  check(ret != -1, "Writing syn failed: %s", strerror(errno));

  return 0;

error:
  return ret;
}

static int create_device(fd) 
{
  int ret;
  struct uinput_user_dev uidev;

  memset(&uidev, 0, sizeof(uidev));
  snprintf(uidev.name, UINPUT_MAX_NAME_SIZE, PROGRAMNAME);

  ret = write(fd, &uidev, sizeof(uidev));
  check(ret != -1, "Failed to write to data to uniput: %s", strerror(errno));

  ret = ioctl(fd, UI_DEV_CREATE);
  check(ret != -1, "ioctl UI_DEV_CREATE failed. %s", strerror(errno));

  return 0;

error:
  return ret;
}

static int configure_device(fd) 
{
  int ret;
  int i;

  ret = ioctl(fd, UI_SET_EVBIT, EV_KEY);
  check(ret != -1, "ioctl UI_SET_EVBIT, EV_KEY failed. %s", strerror(errno));

  ret = ioctl(fd, UI_SET_EVBIT, EV_SYN);
  check(ret != -1, "ioctl UI_SET_EVBIT, EV_SYN failed. %s", strerror(errno));

  for(i = 0; i < KEY_CNT; i++) {
    ret = ioctl(fd, UI_SET_KEYBIT, i);
    check(ret != -1, "ioctl UI_SET_EVBIT, %d failed. %s", i, strerror(errno));
  }

  return 0;

error:
  return ret;
}

static int open_uinput() 
{
  int fd;
  fd = open("/dev/uinput", O_WRONLY);
  if(fd == -1)
    fd = open("/dev/input/uinput", O_WRONLY);

  return fd;
}

int inject_init()
{
  int ret = 0;
  int fd;

  ret = open_uinput();
  if(ret < 0)
    goto error;
  else
    fd = ret;

  ret = configure_device(fd);
  if(ret)
    goto error_close_uinput;

  ret = create_device(fd);
  if(ret)
    goto error_close_uinput;

  return fd;

error_close_uinput:
  close(fd);
error:
  return ret; 
}

int inject_forward_keypress(int fd, int code, int value)
{
  int type = EV_KEY;
  debug("Injecting generic keypress: type=%d, code=%d, value=%d", type, code, value);
  return inject_event(fd, type, code, value);
}

int inject_syn(int fd)
{
  int type = EV_SYN;
  int code = 0;
  int value = 0;
  debug("Injecting SYN");
  return inject_event(fd, type, code, value);
}

int inject_lctrl_down(int fd) 
{
  int type = EV_KEY; 
  int code = KEY_LEFTCTRL; 
  int value = KEY_VALUE_PRESS;
  debug("Injecting LCTRL DOWN");
  return inject_event(fd, type, code, value);
}

int inject_lctrl_up(int fd) 
{
  int type = EV_KEY; 
  int code = KEY_LEFTCTRL; 
  int value = KEY_VALUE_RELEASE;
  debug("Injecting LCTRL UP");
  return inject_event(fd, type, code, value);
}

int inject_escape_down(int fd)
{
  int type = EV_KEY; 
  int code = KEY_ESC; 
  int value = KEY_VALUE_PRESS;
  debug("Injecting ESC DOWN");
  return inject_event(fd, type, code, value);
}

int inject_escape_up(int fd)
{
  int type = EV_KEY; 
  int code = KEY_ESC;
  int value = KEY_VALUE_RELEASE;
  debug("Injecting ESC UP");
  return inject_event(fd, type, code, value);
}
